#pragma once

#include "core/typedefs.h"
#include "core/data/vector.h"

class String {
private:

	Vector<char> _data;
public:

	FORCE_INLINE char *ptrw() { return _data.ptrw(); }
	FORCE_INLINE const char *ptr() const { return _data.ptr(); }
	FORCE_INLINE Error resize(int p_size) { return _data._resize(p_size); }

	FORCE_INLINE char get(int p_index) const { return _data[p_index]; }
	FORCE_INLINE void set(int p_index, char p_item) { _data[p_index] = p_item; }
	FORCE_INLINE int size() const { return _data.size(); }
	FORCE_INLINE void clear() { _data.clear(); }
	FORCE_INLINE bool is_empty() const { return length() == 0; }

	FORCE_INLINE char &operator[](int p_index) {
		return _data[p_index];
	}

	FORCE_INLINE const char &operator[](int p_index) const {
		return _data[p_index];
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

	operator const char *() const { return get_data(); }

	String(const String &p_from);
	String (const char *p_from) { copy_from(p_from); }
	String() {}
protected:

	void copy_from(const char *p_str);
	void copy_from_unchecked(const char *p_str, const int p_length);
};

String operator+(const char *p_lhs, const String &p_rhs);

String vformat(const char *p_string, ...);