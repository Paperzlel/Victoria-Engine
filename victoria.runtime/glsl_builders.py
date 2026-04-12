import os
import sys
import re
from enum import Enum

class ShaderDirective(Enum):
	SHADER_STAGE_VERTEX = 0
	SHADER_STAGE_FRAGMENT = 1
	SHADER_STAGE_MAX = 2
	def to_int(self) -> int:
		return self.value
	def get_string(self) -> str:
		match (self.value):
			case ShaderDirective.SHADER_STAGE_VERTEX:
				return "vertex"
			case ShaderDirective.SHADER_STAGE_FRAGMENT:
				return "fragment"
			case _:
				return "unknown"

def shader_to_int(value: int) -> str:
	match (value):
		case 0:
			return "CANVAS_DATA"
		case _:
			return "UNKNOWN"

def convert_glsl_to_gen_h(file: str):
	name: str = os.path.basename(file).split('/')[-1].removesuffix(".glsl")
	source_file = open(file, 'r', encoding='utf-8')
	stage: ShaderDirective = ShaderDirective.SHADER_STAGE_MAX
	# NOTE: Other shader stages act differently
	data: list = ["", ""]
	uniforms: list[str] = []
	ubos_found: list[int] = []
	ubo_name: list[str] = []
	# Assumes python 3.8
	for line in source_file:
		linetmp: str = line.strip()
		if linetmp.startswith("//") or linetmp.startswith("/*") or linetmp.endswith("*/"):
			continue
		# Skip line if commented out.
		# Convert directive if present
		if "#[" in line:
			stage = check_directive(line)
			continue
		
		if "uniform" in line:
			if line.find("// ubo:") != -1:
				# is a UBO, add to list
				tmp: list[str] = line.split(" ")
				ubo_idx: int = int(tmp[-1].removeprefix("ubo:"))
				# Add UBO if unique
				if not ubo_idx in ubos_found:
					ubos_found.append(ubo_idx)
					ubo_name.append(tmp[-4])
			else:
				tmp: str = line.split(" ")[-1].removesuffix(";\n")
				# Add uniform if unique
				if not tmp in uniforms:
					uniforms.append(tmp)

		if line != "\n":
			data[stage.to_int()] += line
	source_file.close()
	# Now data is collated, write it into the file
	dest_file = open(file.replace(".glsl", ".gen.h"), 'w', encoding='utf-8')
	dest_file.write("#pragma once\n\n")
	dest_file.write("#include \"rendering/opengl/shaders_gl.h\"\n\n")
	
	# Write shader class
	name_pascal: str = name.title()
	name_pascal = "GL" + name_pascal + "Shader"

	dest_file.write(f"class {name_pascal} : public GLShader")
	dest_file.write(" {\npublic:\n\n\tenum Uniforms {\n")
	for uniform in uniforms:
		dest_file.write(f"\t\t{uniform.upper()},\n")
	dest_file.write("\t\tUNIFORMS_MAX\n\t};\n\n")

	# Write UBOS
	dest_file.write("\tenum UBOS {\n")
	for i in range(len(ubos_found)):
		dest_file.write(f"\t\t{re.sub("(?<!^)(?=[A-Z])", "_", ubo_name[i]).upper()} = {ubos_found[i]},\n")
	dest_file.write(f"\t\tUBO_COUNT = {len(ubos_found)}\n")
	dest_file.write("\t};\n\n")

	# Write create step
	dest_file.write("\tvirtual void _init() override {\n")
	dest_file.write("\t\tstatic const char *uniform_names[UNIFORMS_MAX] = {\n")

	for uniform in uniforms:
		dest_file.write(f"\t\t\t\"{uniform}\",\n")
	dest_file.write("\t\t};\n")
	dest_file.write("\t\tstatic UBO ubos[UBO_COUNT] = {\n")

	for i in range(len(ubos_found)):
		dest_file.write("\t\t\t{")
		dest_file.write(f"\"{ubo_name[i]}\", {ubos_found[i]}")
		dest_file.write("},\n")
	dest_file.write("\t\t};\n")

	shader_write_stage_data(dest_file, data[0], "vertex")
	shader_write_stage_data(dest_file, data[1], "fragment")

	dest_file.write("\n\t\t_setup(vertex_data, fragment_data, uniform_names, UNIFORMS_MAX, ubos, UBO_COUNT);\n")
	dest_file.write("\t}\n")
	dest_file.write("};\n")

	dest_file.close()

def check_directive(line: str) -> ShaderDirective:
	line = line.strip()
	line = line.removeprefix("#[").removesuffix("]")
	match line:
		case "vertex":
			return ShaderDirective.SHADER_STAGE_VERTEX
		case "fragment":
			return ShaderDirective.SHADER_STAGE_FRAGMENT
	print("Invalid shader stage, found \"" + line + "\"")
	return ShaderDirective.SHADER_STAGE_MAX

def shader_write_stage_data(handle, data, stage):
	if data != "":
		split_lines = data.split("\n")
		data = "(R\"<!>("
		for line in split_lines:
			if line != "":
				data += f"{line}\n"
		data = data.removesuffix("\n")
		data += ")<!>\")\n"

	handle.write(f"\t\tstatic const char {stage}_data[] = ")
	handle.write("{\n")
	handle.write(data)
	handle.write("\t\t};\n")


# Convert CWD to the executing file
current_path = os.path.dirname(os.path.realpath(__file__))
os.chdir(current_path)

# Assume argument 1 is the name of the file the generate, which is the relative path to the CWD.
convert_glsl_to_gen_h(sys.argv[1])
