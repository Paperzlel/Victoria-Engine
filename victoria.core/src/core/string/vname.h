#pragma once

#include "core/data/atomic_counter.h"
#include "core/string/vstring.h"
#include "core/typedefs.h"

/**
 * Class that implements a speed-based string comparison system. Strings are cached on construction and share the same
 * pointer between objects, hence making the string "name" lookup incredibly fast compared to traditional lookup
 * speeds.
 */
class VAPI VName {
	struct Tree;

	struct Data {
		Refcount refcount;
		String string;
		uint32_t hash;

		struct Data *next = nullptr;
		struct Data *prev = nullptr;
	};

	// Pointer to the actual data held.
	Data *_data = nullptr;

	friend void register_core_types();
	friend void unregister_core_types();
	static void setup();
	static void finalize();

	/**
	 * @brief Unreferences the data held by the `VName`. If the reference count goes to 0 or below, the data is freed.
	 *
	 */
	void unref();

public:
	/**
	 * @brief Obtains the length of the string that this `VName` shadows.
	 * @return The length of the string, or 0 if there is no valid data.
	 */
	int32_t length() const;

	/**
	 * @brief Gets the pointer to the string data held by the object. Such a pointer should not be modified as it
	 * shadows a `String` object.
	 * @return A pointer to the raw string data, or `nullptr` if no data exists.
	 */
	const char *get_data() const;

	char operator[](int p_index) const;

	/**
	 * @brief Gets the current name as a string object. To ensure immutability, the name is copied over instead of
	 * being referenced.
	 * @return A copy of the string shadowed by this `VName`.
	 */
	FORCE_INLINE String get_string() const {
		if (_data) {
			return _data->string;
		}

		return String();
	}

	FORCE_INLINE bool operator==(const VName &p_string) const {
		return _data == p_string._data;
	}

	FORCE_INLINE bool operator!=(const VName &p_string) const {
		return _data != p_string._data;
	}

	FORCE_INLINE bool operator==(const String &p_string) const {
		if (_data && _data->string == p_string) {
			return true;
		}

		return false;
	}

	FORCE_INLINE bool operator!=(const String &p_string) const {
		return !(operator==(p_string));
	}

	FORCE_INLINE bool operator==(const char *p_string) const {
		if (_data && _data->string == p_string) {
			return true;
		}

		return false;
	}

	FORCE_INLINE bool operator!=(const char *p_string) const {
		return !(operator==(p_string));
	}

	void operator=(const VName &p_other) {
		if (_data == p_other._data) {
			return;
		}

		unref();
		if (!p_other._data) {
			_data = nullptr;
			return;
		}

		if (p_other._data->refcount.ref()) {
			_data = p_other._data;
		}
	}

	void operator=(VName &&p_other) {
		if (_data == p_other._data) {
			return;
		}

		unref();
		_data = p_other._data;
		p_other._data = nullptr;
	}

	explicit VName(const VName &p_other) {
		if (!p_other._data) {
			return;
		}

		if (p_other._data->refcount.ref()) {
			_data = p_other._data;
		}
	}

	VName(VName &&p_other) {
		_data = p_other._data;
		p_other._data = nullptr;
	}

	VName(const String &p_other);

	VName(const char *p_string);

	VName() {}

	~VName() {
		if (_data) {
			unref();
		}
	}
};

// Comparison operators for when they're on a different side of the equation.

VAPI bool operator==(const String &p_left, const VName &p_right);
VAPI bool operator!=(const String &p_left, const VName &p_right);
VAPI bool operator==(const char *p_left, const VName &p_right);
VAPI bool operator!=(const char *p_left, const VName &p_right);
