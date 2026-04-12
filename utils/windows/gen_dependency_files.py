import os
import sys
import json
import pathlib

# Run by MSVC includers.
# Looks for "output_file.o" from input.

source: str = ""
file: str = ""
for i, arg in enumerate(sys.argv):
	if arg == "-o":
		file = sys.argv[i + 1]
		continue
	if arg.endswith(".c") or arg.endswith(".cpp") or arg.endswith(".cc"):
		source = arg
		continue

if not file.endswith(".o"):
	print(file)
	print("File to obtain must be the related object file.")
	exit(1)

jfile: str = file.replace(".o", ".cpp.json")
if not os.path.isfile(jfile):
	jfile = file.replace(".o", ".c.json")
	if not os.path.isfile(jfile):
		print(f"No corresponding JSON file for object file {file}")
		exit(2)

# Load JSON data
data: dict = {}
with open(jfile, 'r', encoding='utf-8') as f:
	buf = f.read()
	data = json.loads(buf)
	f.close()

if data.get("Data") is None:
	print("No valid data section could be found.")
	exit(3)

if dict(data.get("Data")).get("Includes") is None:
	print("No valid includes could be found.")
	exit(3)

includes: list[str] = dict(data.get("Data")).get("Includes")
ilen: int = len(includes)
with open(file.replace(".o", ".d"), 'w', encoding='utf-8') as f:
	f.write(f"{file}: {source} \\\n")
	for i, inc in enumerate(includes):
		# Add backslash character to spaces, complies with .d files for Windows.
		inc = str(pathlib.Path(inc).resolve()).replace(" ", "\\ ")
		f.write(inc)
		if i < ilen - 1:
			f.write(' \\\n')
	f.write("\n")
	f.close()