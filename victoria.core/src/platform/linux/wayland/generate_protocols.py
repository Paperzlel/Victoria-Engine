import subprocess
import os


def create_xml(name, path):
    subprocess.call(
        f"wayland-scanner -c client-header < {path} > src/platform/linux/wayland/protocols/{name}.gen.h",
        shell=True,
    )
    subprocess.call(
        f"wayland-scanner -c private-code < {path} > src/platform/linux/wayland/protocols/{name}.gen.c",
        shell=True,
    )

def generate_sources():
    # Generate code protocol
    create_xml("wayland", "src/thirdparty/wayland/protocol/wayland.xml")
    # Stable protocols
    create_xml(                                                                                                                     
        "linux_dmabuf",
        "src/thirdparty/wayland-protocols/stable/linux-dmabuf/linux-dmabuf-v1.xml",
    )
    create_xml(
        "tablet",
        "src/thirdparty/wayland-protocols/stable/tablet/tablet-v2.xml",
    )
    create_xml(
        "xdg_shell",
        "src/thirdparty/wayland-protocols/stable/xdg-shell/xdg-shell.xml",
    )
    # Staging protocols
    create_xml(
        "cursor_shape",
        "src/thirdparty/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml"
    )
    # Unstable protocols
    create_xml(
        "xdg_decoration",
        "src/thirdparty/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml",
    )


# Since getting Make to generate a folder and its contents without doing it twice is a little finnicky,
# we opt to instead do so in Python to alleviate the stresses of getting the damn thing to work.

# NOTE: Make is unable (as far as my testing is concerned) to detect changes to our Python scripts,
# so any additions to this folder require a full rebuild (or at least the deletion of the "protocols" 
# folder.)
protocol_path = os.path.dirname(__file__) + "/protocols"
if not os.path.isdir(protocol_path):
    os.mkdir(protocol_path)
    pass

protocols_dont_exist = True
for _in in os.scandir(protocol_path):
    protocols_dont_exist = False
    break

if protocols_dont_exist:
    generate_sources()
