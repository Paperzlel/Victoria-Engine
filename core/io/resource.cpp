#include "resource.h"

String Resource::get_name() const {
    return name;
}

void Resource::set_name(const String &p_name) {
    name = p_name;
}

String Resource::get_path() const {
    return path;
}

void Resource::set_path(const String &p_path) {
    path = p_path;
}
