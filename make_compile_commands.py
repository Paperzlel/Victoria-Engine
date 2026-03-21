import os
import sys
import json

real_file: str = os.path.dirname(__file__) + "/compile_commands.json"

def begin_compile_commands() -> None:
	file = open(real_file, 'w', encoding='utf=8')
	file.write("[\n")
	file.close()

def end_compile_commands() -> None:
	file = open(real_file, 'a+', encoding='utf-8')
	size = os.path.getsize(real_file)
	file.seek(size - 2, 0)
	file.truncate()
	file.write("\n]\n")

def add_compile_command(args: list, directory: str, file: str, output: str) -> None:
	out_data: dict = {}
	out_data["arguments"] = args
	out_data["directory"] = directory
	out_data["file"] = file
	out_data["output"] = output

	stringified_dict = json.dumps(out_data, indent=2)
	tmp_str: list = stringified_dict.split("\n")
	stringified_dict = ""
	for i in range(len(tmp_str)):
		if i < len(tmp_str) - 1:
			tmp_str[i] = "  " + tmp_str[i] + "\n"
		else:
			tmp_str[i] = "  " + tmp_str[i]
		stringified_dict += tmp_str[i]
	file = open(real_file, 'a', encoding='utf-8')
	file.write(stringified_dict)
	file.write(",\n")
	file.close()

arguments: list = []
cwd: str = ""
working_file: str = ""
output: str = ""
is_collating_args: bool = False
exit_early: bool = False
current_arg: int = 0
for arg in sys.argv:
	match arg:
		case "--begin":
			begin_compile_commands()
			exit_early = True
		case "--end":
			end_compile_commands()
			exit_early = True
		case "--args":
			is_collating_args = True
		case "--file":
			is_collating_args = False
			working_file = sys.argv[current_arg + 1]
		case "--output":
			is_collating_args = False
			output = sys.argv[current_arg + 1]
		case "--directory":
			is_collating_args = False
			cwd = sys.argv[current_arg + 1]
		case _:
			if is_collating_args:
				arguments.append(arg)
	if exit_early:
		break
	current_arg += 1

if not exit_early:
	add_compile_command(arguments, cwd, working_file, output)

