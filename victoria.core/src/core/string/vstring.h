#pragma once

#include "core/data/cowdata.h"
#include "core/data/vector.h"
#include "core/typedefs.h"

/**
 * NOTE: We can create strings that are either 8, 16, or 32-bit encoding (using char32_t instead of char)
 *      For now, they will be 8-bit (to keep size down), but in the future can be expected to be made larger
 *      for localization to other languages (and proper Unicode support)
 */

class VCORE_API String {
private:
	CoWData<char> _data;

public:
	FORCE_INLINE char *ptrw() {
		return _data.ptrw();
	}
	FORCE_INLINE const char *ptr() const {
		return _data.ptr();
	}
	FORCE_INLINE Error resize(int p_size) {
		return _data.resize(p_size);
	}

	FORCE_INLINE char get(int index) const {
		return _data.get(index);
	}
	FORCE_INLINE void set(int index, char p_item) {
		_data.set(p_item, index);
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
		return _data.get(index);
	}

	FORCE_INLINE const char &operator[](int index) const {
		return _data.get(index);
	}

	FORCE_INLINE int length() const {
		int s = size();
		return s ? (s - 1) : 0;
	}

	const char *get_data() const;

	void operator=(const String &p_right);
	void operator=(const char *p_right);

	void operator=(String &&p_right) {
		_data = std::move(p_right._data);
	}

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

	/**
	 * @brief Finds the last instance of the given phrase in the string. If the phrase doesn't exist, returns `-1`.
	 * @param p_what The phrase to look for.
	 * @return `-1` on failure, and the starting index of the first index on success.
	 */
	int rfind(const String &p_what) const;

	String left(int p_count) const;
	String right(int p_count) const;

	bool begins_with(const String &p_string) const;
	bool ends_with(const String &p_string) const;

	String remove_prefix(const String &p_prefix) const;
	String remove_suffix(const String &p_suffix) const;

	Vector<String> split(const String &delimiter) const;
	String substr(int p_from, int p_chars) const;

	void replace(char p_value, char p_replacement);

	String get_file() const;
	String get_file_extension() const;

	/**
	 * @brief Gets the directory containing the given file from the given path. If the path is empty or not a valid
	 * filepath, returns a blank string.
	 * @return The directory correlating to the given path.
	 */
	String get_directory() const;

	/**
	 * @brief Obtains a copy of the given string as a POSIX-compliant path. All paths should be in this form, as it is
	 * the one expected by the engine.
	 * @return A copy of the string as a POSIX-compliant path.
	 */
	String fix_path() const;

	int64_t to_int() const;
	double to_float() const;

	void append(const String &p_string);

	static uint32_t hash(const char *p_cstr);
	uint32_t hash() const;

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

VCORE_API String operator+(const char *p_lhs, const String &p_rhs);
VCORE_API String itos(int64_t p_int);
VCORE_API String ftos(double p_double);

VCORE_API String vformat(const char *p_string, ...);

VCORE_API bool vstring_compare(const char *p_lhs, const char *p_rhs);
