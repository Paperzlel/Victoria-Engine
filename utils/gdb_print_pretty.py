import re 

import gdb
import gdb.printing

class VictoriaStringVisualizer:
	def __init__(self, val):
		self.value = val
	
	def display_hint(self):
		return 'string'
	
	def to_string(self):
		ptr = self.value["_data"]["_ptr"]
		if ptr == 0:
			return ""
		
		ret = ''
		i = 0
		while True:
			ch = int((ptr + i).dereference())

			if ch == 0:
				break

			ret += chr(ch)
			i += 1
		return ret

	
class VictoriaVectorVisualizer:
	def __init__(self, value):
		self.value = value
	
	def display_hint(self):
		return 'array'

	def get_size(self):
		data = self.value["_cowdata"]
		if not data["_ptr"]:
			return 0
		ptr = data["_ptr"].cast(gdb.lookup_type("uint8_t").pointer())
		# Assume size remains at 16 bytes prior to the vector.
		return int((ptr - 8).dereference())
		
	def children(self):
		ptr = self.value["_cowdata"]["_ptr"]
		if int(ptr) == 0:
			return
		for i in range(self.get_size()):
			yield str(i), (ptr + i).dereference()
		
	def to_string():
		return "%s [%d]" % (self.value.type.name, self.get_size())

REGEX = re.compile("^Vector<.*$")

def lookup_printer(value):
	if value.type.name == "String":
		return VictoriaStringVisualizer(value)
	if value.type.name:
		if REGEX.match(value.type.name):
			return VictoriaVectorVisualizer(value)
	return None

gdb.printing.register_pretty_printer(None, lookup_printer)
