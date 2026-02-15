import subprocess


def create_xml(name, path):
    subprocess.call(
        f"wayland-scanner -c client-header < {path} > src/platform/linux/wayland/protocols/{name}.gen.h",
        shell=True,
    )
    subprocess.call(
        f"wayland-scanner -c private-code < {path} > src/platform/linux/wayland/protocols/{name}.gen.c",
        shell=True,
    )


# Generate code protocol
create_xml("wayland", "src/thirdparty/wayland/protocol/wayland.xml")
# Stable protocols
create_xml(
    "linux_dmabuf",
    "src/thirdparty/wayland-protocols/stable/linux-dmabuf/linux-dmabuf-v1.xml",
)
create_xml(
    "xdg_shell",
    "src/thirdparty/wayland-protocols/stable/xdg-shell/xdg-shell.xml",
)
# Unstable protocols
create_xml(
    "xdg_decoration",
    "src/thirdparty/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml",
)
