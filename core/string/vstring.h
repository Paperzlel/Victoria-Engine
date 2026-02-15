#pragma once

#include "core/typedefs.h"
#include "core/data/vector.h"

class String {
private:

	// Internal array of data.
	Vector<char> _data;
public:

	/**
	 * @brief Obtains the raw pointer used by the string with write ability.
	 * @return The pointer to the string with write ability.
	 */
	FORCE_INLINE char *ptrw() { return _data.ptrw(); }
	/**
	 * @brief Obtains the pointer to the string without write ability.
	 * @return The pointer to the string without write ability.
	 */
	FORCE_INLINE const char *ptr() const { return _data.ptr(); }
	/**
	 * @brief Resizes the string to be of a new size `p_size`. Elements beyond the current length may not be resized.
	 * @param p_size The new size of the string in bytes
	 * @return `OK` on success and an error on failure.
	 */
	FORCE_INLINE Error resize(int p_size) { return _data._resize(p_size); }

	/**
	 * @brief Obtains the given character at a specific index.
	 * @param p_index The character index to look up
	 * @return The character at the given index
	 */
	FORCE_INLINE char get(int p_index) const { return _data[p_index]; }
	/**
	 * @brief Sets the character at the given index to use the given item
	 * @param p_index The index to set it at
	 * @param p_item The new character to place at the given index
	 */
	FORCE_INLINE void set(int p_index, char p_item) { _data[p_index] = p_item; }
	/**
	 * @brief Obtains the size of the string in bytes, including the null terminator.
	 * @return The size of the string in bytes
	 */
	FORCE_INLINE int size() const { return _data.size(); }
	/**
	 * @brief Clears all data from the string and sets it to being empty.
	 */
	FORCE_INLINE void clear() { _data.clear(); }
	/**
	 * @brief Checks to see if the given string is empty or not.
	 * @return `true` if yes, `false` if not.
	 */
	FORCE_INLINE bool is_empty() const { return length() == 0; }

	/**
	 * @brief Index operator (non-const)
	 * @param p_index The index to look up
	 * @return Reference to the given character
	 */
	FORCE_INLINE char &operator[](int p_index) {
		return _data[p_index];
	}

	/**
	 * @brief Index operator (const)
	 * @param p_index The index to look up
	 * @return A const reference to the corresponding character
	 */
	FORCE_INLINE const char &operator[](int p_index) const {
		return _data[p_index];
	}

	/**
	 * @brief Obtains the length of the string, i.e. the number of characters not including the NULL terminator.
	 * @returns The number of characters in the string.
	 */
	FORCE_INLINE int length() const {
		int s = size();
		return s ? (s - 1) : 0;
	}

	/**
	 * @brief Obtains the raw string pointer data that can be used by platform-specific or STL code.
	 * @returns The C pointer of the data.
	 */
	const char *get_data() const;

	/**
	 * @brief Assignment operator (our String class)
	 * @param p_right The string to set this string to
	 */
	void operator=(const String &p_right);
	/**
	 * @brief Assignment operator (C strings)
	 * @param p_right The C string to set the data to
	 */
    void operator=(const char *p_right);
	/**
	 * @brief Equivalence operator (our String class)
	 * @param p_right The other string to compare with
	 * @return `true` if equal, `false` if not.
	 */
    bool operator==(const String &p_right) const;
	/**
	 * @brief Equivalence operator (C strings)
	 * @param p_right The other string to compare with
	 * @return `true` if equal, `false` if not.
	 */
    bool operator==(const char *p_right) const;
	/**
	 * @brief Equivalence operator (single char)
	 * @param p_right The other string to compare with
	 * @return `true` if equal, `false` if not.
	 */
    bool operator==(const char p_right) const;
	/**
	 * @brief Non-equivalence operator (our String class)
	 * @param p_right The other string to compare with
	 * @return `true` if not equal, `false` if equal.
	 */
    bool operator!=(const String &p_right) const;
	/**
	 * @brief Non-equivalence operator (C strings)
	 * @param p_right The other string to compare with
	 * @return `true` if not equal, `false` if equal.
	 */
    bool operator!=(const char *p_right) const;
	/**
	 * @brief Non-equivalence operator (single char)
	 * @param p_right The other string to compare with
	 * @return `true` if not equal, `false` if equal.
	 */
    bool operator!=(const char p_right) const;

	/**
	 * @brief String appendage operator (our String class)
	 * @param p_right The string to add onto our own
	 * @return Reference to the new string
	 */
	String &operator+=(const String &p_right);
	/**
	 * @brief String appendage operator (single char)
	 * @param p_right The character to append
	 * @returns Reference to the new string
	 */
    String &operator+=(const char p_right);
	/**
	 * @brief String concatenation operator
	 * @param p_right The other string to append
	 * @return A copy of the two strings appended
	 */
    String operator+(const String &p_right);

	/**
	 * @brief Obtains the string as a C array.
	 */
	operator const char *() const { return get_data(); }

	/**
	 * @brief String class constructor.
	 * @param p_from String to create from
	 */
	String(const String &p_from);
	/**
	 * @brief String class constructor
	 * @param p_from C string to copy from
	 */
	String (const char *p_from) { copy_from(p_from); }
	/**
	 * @brief String class constructor.
	 */
	String() {}
protected:

	/**
	 * @brief Copies data from a C string into our own strings. This version checks to see if the string is of a suitable size and if
	 * the pointer is a valid one.
	 * @param p_str The string to copy from
	 */
	void copy_from(const char *p_str);
	/**
	 * @brief Copies data from a C string into our own string format. This version doesn't check the format of the data or if it's a
	 * valid pointer, so check those prior to use.
	 * @param p_str The C string to copy data from
	 * @param p_length The length of the string
	 */
	void copy_from_unchecked(const char *p_str, const int p_length);
};

/**
 * @brief Concatenates a C string and one of our own String types.
 * @param p_lhs The left hand C string
 * @param p_rhs The right hand string of our String type
 * @return The now-concatenated string.
 */
String operator+(const char *p_lhs, const String &p_rhs);

/**
 * @brief Formats a C string into one of our own datatypes, using variadic arguments as input.
 * @param p_string The string to format
 * @param ... The variadic arguments to use
 * @return The now-formatted string
 */
String vformat(const char *p_string, ...);