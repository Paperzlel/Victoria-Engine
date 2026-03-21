#pragma once

#include "core/error/error_types.h"
#include "core/typedefs.h"

class Variant;
class ArrayData;

class VAPI Array {
private:
	mutable ArrayData *_data;

	void _ref(const Array &p_from) const;
	void _unref() const;

public:
	struct Iterator {
		FORCE_INLINE Variant &operator*() const;
		FORCE_INLINE Variant *operator->() const;

		FORCE_INLINE Iterator &operator++();
		FORCE_INLINE Iterator &operator--();

		FORCE_INLINE bool operator==(const Iterator &p_other) const {
			return _p == p_other._p;
		}
		FORCE_INLINE bool operator!=(const Iterator &p_other) const {
			return _p != p_other._p;
		}

		FORCE_INLINE Iterator(Variant *p_element_ptr) {
			_p = p_element_ptr;
		}
		FORCE_INLINE Iterator() {}
		FORCE_INLINE Iterator(const Iterator &p_other) {
			_p = p_other._p;
		}

	private:
		Variant *_p = nullptr;
	};

	struct ConstIterator {
		FORCE_INLINE const Variant &operator*() const;
		FORCE_INLINE const Variant *operator->() const;

		FORCE_INLINE ConstIterator &operator++();
		FORCE_INLINE ConstIterator &operator--();

		FORCE_INLINE bool operator==(const ConstIterator &p_other) const {
			return _p == p_other._p;
		}
		FORCE_INLINE bool operator!=(const ConstIterator &p_other) const {
			return _p != p_other._p;
		}

		FORCE_INLINE ConstIterator(const Variant *p_element_ptr) {
			_p = p_element_ptr;
		}
		FORCE_INLINE ConstIterator() {}
		FORCE_INLINE ConstIterator(const ConstIterator &p_other) {
			_p = p_other._p;
		}

	private:
		const Variant *_p = nullptr;
	};

	Iterator begin();
	Iterator end();

	ConstIterator begin() const;
	ConstIterator end() const;

	Variant &operator[](uint64_t p_index);
	const Variant &operator[](uint64_t p_index) const;

	void operator=(const Array &p_other);

	bool operator==(const Array &p_other);
	bool operator!=(const Array &p_other);

	int64_t size() const;
	Error resize(int p_new_size);
	bool is_empty() const;
	bool is_read_only() const;
	void set_read_only(bool p_bool) const;

	void append(const Variant &p_item);
	void remove_at(uint64_t p_index);
	void insert(uint64_t p_index, const Variant &p_item);

	Variant pop_front();
	Variant pop_back();
	void push_front(const Variant &p_item);
	void push_back(const Variant &p_item);

	void clear();
	void fill(const Variant &p_item);

	bool is_equal(const Array &p_other) const;

	Array(const Array &p_from);
	Array();

	~Array();
};
