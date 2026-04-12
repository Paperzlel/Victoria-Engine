# Get version info from our OS
version_major = None
version_minor = None
version_patch = None

f = open("engine/include/core/version.h")
contents = f.read()
lines = contents.split("\n")
index = 0
while index < len(lines):
    line = lines[index]

    arr = None
    if line.find("VICTORIA_VERSION_NUMBER_MAJOR") != -1:
        arr = line.split(" ")
        version_major = arr[len(arr) - 1]
    
    if line.find("VICTORIA_VERSION_NUMBER_MINOR") != -1:
        arr = line.split(" ")
        version_minor = arr[len(arr) - 1]
    
    if line.find("VICTORIA_VERSION_NUMBER_PATCH") != -1:
        arr = line.split(" ")
        version_patch = arr[len(arr) - 1]

    if version_major != None and version_minor != None and version_patch != None:
        break
    
    index += 1

print(version_major + "." + version_minor + "." + version_patch)
