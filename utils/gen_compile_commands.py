import os
import sys
import json

cwd: str = os.getcwd()
out_data: list[dict] = []

# Main arguments should be the directories to search (e.g. --search engine editor game)
# For each of those, collect the JSON data from the file and store it locally. Delete the file.
# Now run over each file and do the following:
# - Check for its corresponding .o file in the list. Use the same position as the source file.
# - Check path aginst each argument list. Look for the suffix and the directory. Suffix must match.
# - A "score" is given to the value, which is the length of the similar strings. The greatest
# - length wins and is the arguments used. 
# - Create a dictionary from the data (arguments, source file, directory, output file) and append.
# Once done, output the data to the final JSON file.

def add_file_to_commands(data: dict, directory: str, index: int) -> None:
	source_path, ext = os.path.splitext(data["sources"][index])
	obj_compliant_path: str = source_path.removeprefix("src/")
	out_object: str = ""
	if obj_compliant_path in data["objects"][index]:
		out_object = data["objects"][index]
	else:
		# Slow, don't call except if really needed
		for i in range(len(data["objects"])):
			if obj_compliant_path in data["objects"][i]:
				out_object = data["objects"][i]
				break
	# Look for directory score
	dir_score: int = 0
	dir_index: int = -1
	fileargs: list[str] = []
	for i in range(len(data["args"])):
		argd: dict = data["args"][i]
		if argd["extension"] != ext:
			continue
		if argd["dir"] in source_path:
			new_dir_score: int = len(argd["dir"])
			if new_dir_score > dir_score:
				dir_score = new_dir_score
				dir_index = i
	if dir_index == -1:
		print("Error: No valid argument list found for file", source_path + ext)
		return
	
	new_data: dict = {}
	new_data["arguments"] = data["args"][dir_index]["args"]
	new_data["directory"] = directory
	new_data["file"] = source_path + ext
	new_data["output"] = out_object
	out_data.append(new_data)
			
		

def get_commands_for_target(target: str) -> None:
	directory: str = cwd + "/" + target
	path: str = directory + "/temp.json"
	if not os.path.exists(path):
		# Call recursively to find subfiles
		for subfolder in os.listdir(directory):
			npath: str = os.path.join(target, subfolder)
			if not os.path.isdir(npath):
				continue
			get_commands_for_target(npath)
		return
	tmpfh = open(path, 'r', encoding='utf-8')
	data_dict: dict = json.loads(tmpfh.read())
	tmpfh.close()
	os.remove(path)
	for i in range(len(data_dict["sources"])):
		add_file_to_commands(data_dict, directory, i)


files_to_check: list[str] = []
is_searching: bool = False
for arg in sys.argv:
	if arg == "--search":
		is_searching = True
		continue
	
	if is_searching:
		files_to_check.append(arg)

for target in files_to_check:
	get_commands_for_target(target)

final_fh = open("compile_commands.json", 'w', encoding='utf-8')
final_fh.write(json.dumps(out_data, indent=2))
final_fh.close()
