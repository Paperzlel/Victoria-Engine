#pragma once

#include "core/typedefs.h"
#include "core/variant/variant.h"


VAPI String stringify_variant(const Variant &p_var);

template<typename... Args>
String stringify_variant(const Variant &p_var, Args... p_args) {
    return p_var.operator String() + " " + stringify_variant(p_args...);
}

VAPI extern bool is_printing_verbose();
VAPI extern void _print_line(const String &s);

#define print_verbose(m_text)       \
    if (is_printing_verbose()) {    \
        print_line(m_text);         \
    }

inline void print_line(const Variant &v) {
    _print_line(stringify_variant(v));
}

template<typename... Args>
inline void print_line(const Variant &v, Args... p_args) {
    _print_line(stringify_variant(v, p_args...));
}