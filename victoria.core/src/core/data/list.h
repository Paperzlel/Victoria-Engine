#pragma once

#include "core/error/error_macros.h"
#include "core/os/memory.h"
#include "core/typedefs.h"

template <typename T>
class List {
public:
	struct Element {
	private:
		friend class List<T>;

		T value;
		Element *next_ptr = nullptr;
		Element *prev_ptr = nullptr;

	public:
		FORCE_INLINE Element *next() {
			return next_ptr;
		}

		FORCE_INLINE const Element *next() const {
			return next_ptr;
		}

		FORCE_INLINE Element *prev() {
			return prev_ptr;
		}

		FORCE_INLINE const Element *prev() const {
			return prev_ptr;
		}

		FORCE_INLINE T &get() {
			return value;
		}

		FORCE_INLINE const T &get() const {
			return value;
		}

		Element() {}
		~Element() {}
	};

private:
	Element *head = nullptr;
	Element *tail = nullptr;
	int element_count = 0;

public:
	struct Iterator {
		FORCE_INLINE T &operator*() const {
			return e->get();
		}
		FORCE_INLINE T *operator->() const {
			return &e->get();
		}

		FORCE_INLINE Iterator &operator++() {
			e = e->next();
			return *this;
		}

		FORCE_INLINE Iterator &operator--() {
			e = e->prev();
			return *this;
		}

		FORCE_INLINE bool operator==(const Iterator &p_other) const {
			return e == p_other.e;
		}
		FORCE_INLINE bool operator!=(const Iterator &p_other) const {
			return e != p_other.e;
		}

		Iterator(Element *p_E) {
			e = p_E;
		}
		Iterator() {}
		Iterator(const Iterator &p_iter) {
			e = p_iter.e;
		}

	private:
		Element *e = nullptr;
	};

	FORCE_INLINE Iterator begin() {
		return Iterator(front());
	}

	FORCE_INLINE Iterator end() {
		return Iterator(nullptr);
	}

	struct ConstIterator {
		FORCE_INLINE const T &operator*() const {
			return e->get();
		}
		FORCE_INLINE const T *operator->() const {
			return &e->get();
		}

		FORCE_INLINE ConstIterator &operator++() {
			e = e->next();
			return *this;
		}

		FORCE_INLINE ConstIterator &operator--() {
			e = e->prev();
			return *this;
		}

		FORCE_INLINE bool operator==(const ConstIterator &p_other) const {
			return e == p_other.e;
		}
		FORCE_INLINE bool operator!=(const ConstIterator &p_other) const {
			return e != p_other.e;
		}

		ConstIterator(const Element *p_E) {
			e = p_E;
		}
		ConstIterator() {}
		ConstIterator(const ConstIterator &p_iter) {
			e = p_iter.e;
		}

	private:
		const Element *e = nullptr;
	};

	FORCE_INLINE ConstIterator begin() const {
		return ConstIterator(front());
	}

	FORCE_INLINE ConstIterator end() const {
		return ConstIterator(nullptr);
	}

	FORCE_INLINE Element *front() {
		return head;
	}

	FORCE_INLINE const Element *front() const {
		return head;
	}

	FORCE_INLINE Element *back() {
		return tail;
	}

	FORCE_INLINE const Element *back() const {
		return tail;
	}

	FORCE_INLINE void operator=(const List &p_other) {
		if (this == &p_other) {
			return;
		}

		clear();
		const Element *ptr = p_other.front();
		while (ptr) {
			push_back(ptr->get());
			ptr = ptr->next_ptr;
		}
	}

	FORCE_INLINE void operator=(List &&p_other) {
		if (this == &p_other) {
			return;
		}

		clear();
		head = p_other.head;
		tail = p_other.tail;
		element_count = p_other.element_count;
		p_other.head = nullptr;
		p_other.tail = nullptr;
		p_other.element_count = 0;
	}

	FORCE_INLINE void operator=(std::initializer_list<T> p_init) {
		clear();
		for (const T &elem : p_init) {
			push_back(elem);
		}
	}

	FORCE_INLINE Element *push_front(const T &p_item) {
		Element *e = vnew(Element);
		e->value = p_item;
		e->next_ptr = head;
		e->prev_ptr = nullptr;

		if (head) {
			head->prev_ptr = e;
		}

		head = e;
		if (!tail) {
			tail = e;
		}

		element_count++;
		return e;
	}

	FORCE_INLINE void pop_front() {
		Element *e = front();
		erase(e);
	}

	FORCE_INLINE Element *push_back(const T &p_item) {
		Element *e = vnew(Element);
		e->value = p_item;
		e->prev_ptr = tail;
		e->next_ptr = nullptr;
		if (tail) {
			tail->next_ptr = e;
		}

		tail = e;
		if (!head) {
			head = e;
		}

		element_count++;
		return e;
	}

	FORCE_INLINE void pop_back() {
		Element *e = back();
		erase(e);
	}

	FORCE_INLINE bool erase(Element *p_elem) {
		ERR_COND_NULL_R(p_elem, false);

		if (head == p_elem) {
			head = p_elem->next_ptr;
		}

		if (tail == p_elem) {
			tail = p_elem->prev_ptr;
		}

		if (p_elem->next_ptr) {
			p_elem->next_ptr->prev_ptr = p_elem->prev_ptr;
		}

		if (p_elem->prev_ptr) {
			p_elem->prev_ptr->next_ptr = p_elem->next_ptr;
		}

		vdelete(p_elem);
		p_elem = nullptr;
		element_count--;
		return true;
	}

	FORCE_INLINE void clear() {
		while (front()) {
			erase(front());
		}
	}

	FORCE_INLINE bool erase(const T &p_item) {
		Element *e = find(p_item);
		return erase(e);
	}

	FORCE_INLINE Element *find(const T &p_item) {
		Element *e = front();
		while (e) {
			if (e->value == p_item) {
				return e;
			}
			e = e->next();
		}

		return nullptr;
	}

	FORCE_INLINE bool has(const T &p_item) {
		return find(p_item) != nullptr;
	}

	T &get(int p_index) {
		CRASH_COND_MSG(p_index >= size(), "Given index was out of bounds.");
		Element *I = front();
		int c = 0;
		while (c < p_index) {
			I = I->next();
			c++;
		}

		return I->get();
	}

	const T &get(int p_index) const {
		CRASH_COND_MSG(p_index >= size(), "Given index was out of bounds.");
		const Element *I = front();
		int c = 0;
		while (c < p_index) {
			I = I->next();
			c++;
		}

		return I->get();
	}

	FORCE_INLINE int size() const {
		return element_count;
	}

	FORCE_INLINE bool is_empty() const {
		return element_count == 0;
	}

	List() {}

	explicit List(const List &p_other) {
		const Element *ptr = p_other.front();
		while (ptr) {
			push_back(ptr->get());
			ptr = ptr->next_ptr;
		}
	}

	List(List &&p_other) {
		head = p_other.head;
		tail = p_other.tail;
		element_count = p_other.element_count;
		p_other.head = nullptr;
		p_other.tail = nullptr;
		p_other.element_count = 0;
	}

	List(std::initializer_list<T> p_init) {
		for (const T &elem : p_init) {
			push_back(elem);
		}
	}

	~List() {
		clear();
	}
};
