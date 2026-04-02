#pragma once

#include "core/data/vector.h"
#include "core/typedefs.h"

/**
 * NOTE: We can create strings that are either 8, 16, or 32-bit encoding (using char32_t instead of char)
 *      For now, they will be 8-bit (to keep size down), but in the future can be expected to be made larger
 *      for localization to other languages (and proper Unicode support)
 */

class VAPI String {
private:
	Vector<char> _data;

public:
	FORCE_INLINE char *ptrw() {
		return _data.ptrw();
	}
	FORCE_INLINE const char *ptr() const {
		return _data.ptr();
	}
	FORCE_INLINE Error resize(int p_size) {
		return _data._resize(p_size);
	}

	FORCE_INLINE char get(int index) const {
		return _data[index];
	}
	FORCE_INLINE void set(int index, char p_item) {
		_data[index] = p_item;
	}
	FORCE_INLINE int size() const {
		return _data.size();
	}
	FORCE_INLINE void clear() {
		_data.clear();
	}
	FORCE_INLINE bool is_empty() const {
		return length() == 0;
	}

	FORCE_INLINE char &operator[](int index) {
		return _data[index];
	}

	FORCE_INLINE const char &operator[](int index) const {
		return _data[index];
	}

	FORCE_INLINE int length() const {
		int s = size();
		return s ? (s - 1) : 0;
	}

	const char *get_data() const;

	void operator=(const String &p_right);
	void operator=(const char *p_right);
	bool operator==(const String &p_right) const;
	bool operator==(const char *p_right) const;
	bool operator==(const char p_right) const;
	bool operator!=(const String &p_right) const;
	bool operator!=(const char *p_right) const;
	bool operator!=(const char p_right) const;

	String &operator+=(const String &p_right);
	String &operator+=(const char p_right);
	String operator+(const String &p_right);

	int find(const String &p_what) const;
	bool contains(const String &p_string) const;
	int count(const String &p_what) const;

	String left(int p_count) const;
	String right(int p_count) const;

	bool begins_with(const String &p_string) const;
	bool ends_with(const String &p_string) const;

	String remove_prefix(const String &p_prefix) const;
	String remove_suffix(const String &p_suffix) const;

	Vector<String> split(const String &delimiter) const;
	String substr(int p_from, int p_chars) const;

	void replace(char p_value, char p_replacement);

	String get_file_extension() const;

	int64_t to_int() const;
	double to_float() const;

	void append(const String &p_string);

	operator const char *() const {
		return get_data();
	}

	String(const String &p_from);
	String(const char *p_from) {
		copy_from(p_from);
	}
	String(const char p_from);
	String() {}

protected:
	void copy_from(const char *p_str);
	void copy_from_unchecked(const char *p_str, const int p_length);
};

VAPI String operator+(const char *p_lhs, const String &p_rhs);
VAPI String itos(int64_t p_int);
VAPI String ftos(double p_double);

VAPI String vformat(const char *p_string, ...);
