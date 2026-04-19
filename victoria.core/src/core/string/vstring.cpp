#include "core/string/vstring.h"

#include "core/math/math_funcs.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Returns the raw data of a string for applying in specific C++ circumstances.
 */
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
	_data.resize(lhs_len + rhs_len + 1);

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
		_data.resize(sizeof(char) + 1);
	}

	char *end = ptrw();
	_data.resize(size + 2);

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

/**
 * @brief Finds the first occurence of a given string in another string. In this instance, the string is case-sensitive
 * and will not work if there is a difference in capitalisation.
 * @param p_what The string to look for in the larger string
 * @returns The point in the string where the first character of the string given appears if successful, and `-1` if
 * the string could not be found.
 */
int String::find(const String &p_what) const {
	if (is_empty() || p_what.is_empty()) {
		return -1;
	}

	int pos = -1;
	int similarity = 0;

	for (int i = 0; i <= length(); i++) {
		if (operator[](i) == p_what[similarity]) {
			similarity++;

			if (similarity == p_what.length()) {
				pos = i - similarity + 1;
				break;
			}
		}
	}

	return pos;
}

/**
 * @brief Tells us if the given string has at least one instance of itself within another string.
 * @param p_string The string to look for
 */
bool String::contains(const String &p_string) const {
	return find(p_string) != -1;
}

/**
 * @brief Obtains the number of instances of a given string within a broader string. This version is case-sensitive and
 * will not work if there is a difference in capitalisation.
 */
int String::count(const String &p_what) const {
	const int str_len = length();
	const int other_str_len = p_what.length();
	int first_index = find(p_what);
	// First pass of the string has no values, therefore there is no string to return
	if (first_index == -1) {
		return 0;
	}

	int offset = first_index + other_str_len;
	int count = 1;
	String n_str;

	while (offset < str_len) {
		n_str = substr(offset, str_len - offset);
		int n_offset = n_str.find(p_what);
		if (n_offset == -1) {
			break;
		}

		offset += n_offset + other_str_len;
		count++;
	}

	return count;
}

/**
 * @brief Returns a copy of the current string that is the first n characters in the given string (left to right). If n
 * is less than 0, then n characters are removed from the end of the string.
 * @param p_count The length of the new string
 * @returns The string as described above
 */
String String::left(int p_count) const {
	if (p_count == 0) {
		return String(*this);
	}
	if (!length()) {
		return String(*this);
	}

	String ret;

	if (p_count < 0) {
		ret.copy_from_unchecked(ptr(), length() + p_count);
	} else {
		ret.copy_from_unchecked(ptr(), p_count);
	}

	return ret;
}

/**
 * @brief Returns a copy of the current string that is the last n characters in the given string (left to right). If n
 * is less than 0, then n characters are removed from the start of the string.
 * @param p_count The length of the new string
 * @returns The string as described above
 */
String String::right(int p_count) const {
	String ret;

	if (p_count == 0) {
		return String(*this);
	}

	if (p_count < 0) {
		ret.copy_from_unchecked(ptr() - p_count, length() + p_count);
	} else {
		ret.copy_from_unchecked(ptr() + (length() - p_count), p_count);
	}

	return ret;
}

/**
 * @brief Checks if the current string begins with a given string.
 * @param p_string The string to compare with
 * @returns `true` if the current string does begin with the given string
 */
bool String::begins_with(const String &p_string) const {
	if (!length()) {
		return false; // String is empty
	}

	int len = p_string.length();

	if (len == 0) {
		return true; // Nothing is an acceptable start to the string, however this may change in the future.
	}

	for (int i = 0; i < len; i++) {
		if (_data[i] != p_string[i]) {
			return false;
		}
	}

	return true;
}

/**
 * @brief Checks if the current string ends with a given string.
 * @param p_string The string to compare with
 * @returns `true if the current string does end with the given string
 */
bool String::ends_with(const String &p_string) const {
	if (!_data.size()) {
		return false;
	}
	int t_len = length();
	int s_len = p_string.length();

	if (s_len == 0) {
		return true;
	}

	int counter = 0;
	for (int i = t_len; i > (t_len - s_len - 1); i--) {
		if (_data[i] == p_string[counter]) {
			counter++;
			if (counter == s_len) {
				return true;
			}
		}
	}

	return false;
}

/**
 * @brief Removes the given string from the start of the current string, if it is found within said string.
 * @param p_prefix The specified string to remove
 * @returns The string with the given prefix removed
 */
String String::remove_prefix(const String &p_prefix) const {
	if (!length()) {
		return String();
	}

	if (!p_prefix.length()) {
		return *this;
	}

	int loc = find(p_prefix);
	if (loc == -1) {
		return *this;
	}

	String ret = right(-p_prefix.length());
	return ret;
}

/**
 * @brief Removes the given string from the end of the current string, it if is found within said string.
 * @param p_suffix The specified string to remove
 * @returns The string with the given suffix removed
 */
String String::remove_suffix(const String &p_suffix) const {
	if (!length()) {
		return String();
	}

	if (!p_suffix.length()) {
		return *this;
	}

	int loc = find(p_suffix);
	if (loc == -1) {
		return *this;
	}

	String ret = left(-p_suffix.length());
	return ret;
}

/**
 * @brief Method to separate one string into several smaller ones, with a delimiter as the indicator for how one wants
 * to split the string.
 * @param delimiter The "phrase" used to determine whether the string should be split
 * @return A `Vector<String>` of individual strings
 */
Vector<String> String::split(const String &delimiter) const {
	Vector<String> ret;

	if (is_empty()) {
		return ret;
	}

	if (delimiter.is_empty()) {
		ret.push_back(String(*this));
		return ret;
	}

	int similarity = 0;
	int from = 0;
	int chars = 0;
	for (int i = 0; i <= length(); i++) {
		// Check if the characters in the string are the same, yes then we substr and append the new item
		if (operator[](i) == delimiter[similarity]) {
			similarity++;
			if (similarity == delimiter.length()) {
				ret.push_back(substr(from, chars));
				from = i + 1;
				chars = 0;
				similarity = 0;
				continue;
			}
		}
		chars++;
	}
	// At the end of the process, we need to add the other string in. - 1 is for the offset to prevent including 2 null
	// terminators
	ret.push_back(substr(from, chars - 1));

	return ret;
}

/**
 * @brief Returns a new, smaller string from a larger string, from a given position
 * @param p_from The smallest index to begin the substring from
 * @param p_chars The number of characters in the string
 */
String String::substr(int p_from, int p_chars) const {
	if (is_empty() || p_from < 0 || p_from > length() || p_chars <= 0) {
		return ""; // do nothin'
	}

	if (p_from == 0 && p_chars >= length()) {
		return String(*this);
	}

	String str;
	str.copy_from_unchecked(&get_data()[p_from], p_chars);
	return str;
}

/**
 * @brief Replaces every instance of a character in a string with another character. If no
 * characters are found of that type, it does nothing.
 * @param p_value The character we want to replace
 * @param p_replacement The replacement character
 */
void String::replace(char p_value, char p_replacement) {
	if (!size()) {
		return;
	}

	if (!contains(p_value)) {
		return;
	}

	for (int i = 0; i < length(); i++) {
		if (_data[i] == p_value) {
			_data[i] = p_replacement;
		}
	}
}

String String::get_file_extension() const {
	// Isn't a filepath, do nothing
	if (!contains(".")) {
		return "";
	}
	Vector<String> vs = split(".");
	return vs[vs.size() - 1];
}

/**
 * @brief Takes a string and turns it into an integer. This applies universally to a string, so any non-numerical
 * characters will still be read as a number, which can confuse the function and give a non-intended result.
 * @returns The given string as an integer.
 */
int64_t String::to_int() const {
	String self = *this;
	int64_t ret = 0;
	// Get length of the string
	// Assume the value is only a number
	int len = length();
	int sign = 1;

	if (begins_with("-")) {
		self = right(-1);
		len -= 1;
		sign = -1;
	}

	for (int i = 0; i < len; i++) {
		char val = self[i];
		uint8_t num = (uint8_t)(val > 0 ? val : 48) - 48;
		ret += (num * Math::pow(10, len - i - 1));
	}
	return ret * sign;
}

/**
 * @brief Converts a regular string into a floating-point number.
 * @returns The current string as a floating-point number
 */
double String::to_float() const {
	double ret = 0.0;

	// Maybe not the best way to do this, but we'll stick at it for now.
	Vector<String> v = split(".");
	ret += (double)(v[0].to_int());

	for (int i = 0; i < v[1].length(); i++) {
		char c = v[1].get(i);
		uint8_t n = (uint8_t)(c > 0 ? c : 48) - 48;
		ret += (double)(n * Math::pow(10, -(i + 1)));
	}

	return ret;
}

void String::append(const String &p_string) {
	int len = length();
	_data.resize(len + p_string.length() + 1);
	for (int i = len; i < len + p_string.length(); i++) {
		_data[i] = p_string[i - len];
	}
}

/**
 * @brief Takes a given integer and converts it into a string.
 */
String itos(int64_t p_int) {
	String ret;
	int magnitude = 0;
	int64_t abs_num = Math::abs(p_int);

	if (p_int < 10 && p_int > -10) {
		magnitude = 1;
	} else {
		while (abs_num / Math::pow(10, magnitude) >= 1) {
			magnitude++;
		}
	}
	// Allocate for the extra negative sign
	if (p_int < 0) {
		magnitude++;
	}

	ret.resize(magnitude + 1);
#ifdef PLATFORM_WINDOWS
	sprintf(ret.ptrw(), "%lli", p_int);
#else
	sprintf(ret.ptrw(), "%li", p_int);
#endif
	return ret;
}

/**
 * @brief Converts a given floating-point number into a string.
 */
String ftos(double p_double) {
	int dp = 14;

	const double abs_val = Math::abs(p_double);
	if (abs_val > 10) {
		dp -= (int)Math::floor(Math::log10(abs_val));
	}

	char fmt[7];
	fmt[0] = '%';
	fmt[1] = '.';
	if (dp < 0) {
		fmt[1] = 'l';
		fmt[2] = 'f';
		fmt[3] = 0;
	} else if (dp < 10) {
		fmt[2] = '0' + dp;
		fmt[3] = 'l';
		fmt[4] = 'f';
		fmt[5] = 0;
	} else {
		fmt[2] = '0' + (dp / 10);
		fmt[3] = '0' + (dp % 10);
		fmt[4] = 'l';
		fmt[5] = 'f';
		fmt[6] = 0;
	}

	char buf[325];

#if _MSC_VER
	snprintf(buf, 325, fmt, p_double);
#else
	sprintf(buf, fmt, p_double);
#endif // _MSC_VER

	// Add null terminator
	buf[324] = 0;

	bool p = false;
	int i = 0;
	while (buf[i]) {
		if (buf[i] == '.') {
			p = true;
		}
		i++;
	}

	if (p) {
		i--;
		while (i > 0) {
			if (buf[i] == '0') {
				buf[i] = 0;
			} else if (buf[i] == '.') {
				buf[i + 1] = '0';
				break;
			} else {
				break;
			}
			i--;
		}
	}

	return buf;
}

/**
 * @brief Class constructor for a string.
 */
String::String(const String &p_from) {
	_data._ref(p_from._data);
}

String::String(const char p_from) {
	resize(2);
	_data[0] = p_from;
	_data[1] = 0;
}

/**
 * @brief Instances a string type with the data from a C string.
 */
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
		uint8_t c = p_str[i];
		if (c == 0 && i < len) {
			dest[i] = ' ';
		} else {
			dest[i] = c;
		}
	}
}

/**
 * @brief Method to copy in a C string to the class from another location. Assumes that we know the string is not null,
 * and that its length is greater than zero, and so forth.
 * @param p_str The C string to copy into the String
 * @param p_length The length of the string (not including null terminator)
 */
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
	while (cstr[n_size]) {
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

bool vstring_compare(const char *p_lhs, const char *p_rhs) {
	return strcmp(p_lhs, p_rhs) == 0;
}
