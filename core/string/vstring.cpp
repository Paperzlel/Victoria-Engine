#include "vstring.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

const char *String::get_data() const {
    if (size()) {
        return &operator[](0);
    } else {
        return "";
    }
}

void String::operator=(const String &p_right) {
    if (*this == p_right) {
        return; // Don't copy if they're pretty much the same.
    }
    _data = p_right._data;
}

void String::operator=(const char *p_right) {
    copy_from(p_right);
}

bool String::operator==(const String &p_right) const {
    return _data == p_right._data;
}

bool String::operator==(const char *p_right) const {
    String rhs = p_right;
    return *this == rhs;
}

bool String::operator==(const char p_right) const {
    if (length() > 1) {
        return false;
    }
    return _data[0] == p_right;
}

bool String::operator!=(const String &p_right) const {
    return !(*this == p_right);
}

bool String::operator!=(const char *p_right) const {
    return !(*this == p_right);
}

bool String::operator!=(const char p_right) const {
    return !(*this == p_right);
}

String &String::operator+=(const String &p_right) {
    if (length() == 0) {
        *this = p_right;
        return *this;
    }
    int lhs_len = length();

    if (p_right.length() == 0) {
        return *this;
    }
    int rhs_len = p_right.length();


    const char *rhs_ptr = p_right.ptr();

    // Resize, so the pointer is large enough
    _data._resize(lhs_len + rhs_len + 1);

    const char *lhs_ptr = ptrw();

    for (int i = 0; i < lhs_len + rhs_len; i++) {
        if (i < lhs_len) {
            _data[i] = lhs_ptr[i];
        } else {
            _data[i] = rhs_ptr[i - lhs_len];
        }
    }

    _data[lhs_len + rhs_len] = '\0';
    return *this;
}

String &String::operator+=(const char p_right) {
    int size = length();

    if (!ptrw()) {
        _data._resize(sizeof(char));
    }

    char *end = ptrw();
    _data._resize(size + 2);
    
    end[size] = p_right;
    end[size + 1] = 0;

    for (int i = 0; i < size + 1; i++) {
        _data[i] = end[i];
    }
    _data[size + 1] = '\0';

    return *this;
}

String String::operator+(const String &p_right) {
    String str = *this;
    str += p_right;
    return str;
}

String operator+(const char *p_lhs, const String &p_rhs) {
    String temp = p_lhs;
    temp += p_rhs;
    return temp;
}

String::String(const String &p_from) {
    _data._ref(p_from._data);
}

void String::copy_from(const char *p_str) {
    if (!p_str) {
        return;
    }

    const int len = strlen(p_str);

    if (len == 0) {
        resize(0);
        return;
    }

    resize(len + 1);
    char *dest = ptrw();

    for (int i = 0; i <= len; i++) {
        uint8 c = p_str[i];
        if (c == 0 && i < len) {
            dest[i] = ' ';
        } else {
            dest[i] = c;
        }
    }
}

void String::copy_from_unchecked(const char *p_str, const int p_length) {
    resize(p_length + 1);
    char *data = ptrw();
    data[p_length] = 0;

    for (int i = 0; i < p_length; i++) {
        data[i] = p_str[i];
    }
}

String vformat(const char *p_string, ...) {
    char cstr[32000];
    memset(cstr, 0, 32000);

    va_list arg_ptr;
    va_start(arg_ptr, p_string);
    vsnprintf(cstr, 32000, p_string, arg_ptr);
    va_end(arg_ptr);

    int n_size = 0;
    while(cstr[n_size]) {
        n_size++;
    }

    String ret;
    ret.resize(n_size + 2);
    for (int i = 0; i < n_size; i++) {
        ret[i] = cstr[i];
    }
	
    ret[n_size + 1] = 0;
    return ret;
}
