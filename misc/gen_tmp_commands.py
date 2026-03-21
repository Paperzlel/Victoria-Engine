import os
import sys
import json

cwd: str = os.getcwd()
# We want to get all data currently in the JSON file prior to appending.
fh = open("temp.json", 'a+', encoding='utf-8')
fh.seek(0, 0)
current_json_string: str = fh.read()
fh.seek(0, 0)
fh.truncate()
output: dict = {}
if (current_json_string != ""):
	output = json.loads(current_json_string)

# Parse command-line args
new_dir: str = ""
file_extension: str = ""
arglist: list[str] = []
source_list: list[str] = []
object_list: list[str] = []
is_getting_args: bool = False
is_getting_sources: bool = False
is_getting_objects: bool = False
for i in range(len(sys.argv)):
	opt: str = sys.argv[i]
	match opt:
		case "--arg_path":
			is_getting_args = False
			is_getting_objects = False
			is_getting_sources = False
			new_dir = sys.argv[i + 1]
			continue
		case "--extension":
			is_getting_args = False
			is_getting_objects = False
			is_getting_sources = False
			file_extension = sys.argv[i + 1]
			continue
		case "--args":
			is_getting_args = True
			is_getting_objects = False
			is_getting_sources = False
			continue
		case "--sources":
			is_getting_sources = True
			is_getting_objects = False
			is_getting_args = False
			continue
		case "--objects":
			is_getting_objects = True
			is_getting_sources = False
			is_getting_args = False
			continue
		case _:
			if is_getting_args:
				arglist.append(opt)
			elif is_getting_sources:
				source_list.append(opt)
			elif is_getting_objects:
				object_list.append(opt)

if output.get("args") is None:
	output["args"] = []

if new_dir != "" and len(arglist) > 0:
	# Assume C++ files
	if file_extension == "":
		file_extension = ".cpp"

	can_create: bool = True
	for d in output["args"]:
		if d["dir"] == new_dir and d["args"] == arglist and d["extension"] == file_extension:
			can_create = False
	if can_create:
		new_arglist = {}
		new_arglist["dir"] = new_dir
		new_arglist["args"] = arglist
		new_arglist["extension"] = file_extension
		output["args"].append(new_arglist)
if len(source_list) > 0:
	output["sources"] = source_list
if len(object_list) > 0:
	output["objects"] = object_list

fh.write(json.dumps(output, indent=2))
