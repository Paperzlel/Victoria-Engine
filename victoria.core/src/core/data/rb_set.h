#pragma once

#include "core/error/error_macros.h"
#include "core/os/memory.h"
#include "core/typedefs.h"

/**
 * Implementation of a RB Map (see https://en.wikipedia.org/wiki/Red%E2%80%93black_tree)
 */

template <typename T>
class RBSet {
	enum Colour {
		RED,
		BLACK
	};

public:
	class Element {
	private:
		friend class RBSet<T>;
		int colour = RED;
		Element *left = nullptr;
		Element *right = nullptr;
		Element *parent = nullptr;
		Element *_next = nullptr;
		Element *_prev = nullptr;
		T _data;

	public:
		T &get() {
			return _data;
		}

		const T &get() const {
			return _data;
		}

		const Element *next() const {
			return _next;
		}

		Element *next() {
			return _next;
		}

		const Element *prev() const {
			return _prev;
		}

		Element *prev() {
			return _prev;
		}
	};

	struct Iterator {
		friend class RBSet<T>;

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

		explicit operator bool() const {
			return e != nullptr;
		}

		Iterator &operator=(const Iterator &p_iter) {
			e = p_iter.e;
			return *this;
		}

		Iterator(Element *p_e) {
			e = p_e;
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
		friend class RBSet<T>;

		FORCE_INLINE const T &operator*() const {
			e->get();
		}

		FORCE_INLINE const T *operator->() const {
			&e->get();
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

		ConstIterator(const Element *p_e) {
			e = p_e;
		}

		ConstIterator() {}
		ConstIterator(const ConstIterator &p_other) {
			e = p_other.e;
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

private:
	Element *_root = nullptr;
	// An element that has all of its data uninitialized. Has no parent or sub-nodes or keys, and is set to black as
	// its default colour.
	Element *_nil = nullptr;
	uint32_t element_count = 0;

	void _create_root() {
		_root = vnew(Element());
		_root->parent = _root->left = _root->right = _nil;
		_root->colour = BLACK;
	}

	void _free_root() {
		if (_root) {
			vdelete(_root);
			_root = nullptr;
		}
	}

	void _create_nil() {
		_nil = (Element *)&DefaultNilClass::_nil;
	}

	void _delete_nil() {
		// vdelete(_nil);
	}

	void _set_colour(Element *p_node, int p_colour) {
		ERR_FAIL_COND(p_node == _nil && p_colour == RED);
		p_node->colour = p_colour;
	}

	inline Element *_successor(Element *p_node) {
		Element *node = p_node;

		if (node->right != _nil) {
			node = node->right;
			while (node->left != _nil) {
				node = node->left;
			}

			return node;
		} else {
			while (node == node->parent->right) {
				node = node->parent;
			}

			if (node->parent == _root) {
				return nullptr;
			}

			return node->parent;
		}
	}

	inline Element *_predecessor(Element *p_node) {
		Element *node = p_node;

		if (node->left != _nil) {
			node = node->left;
			while (node->right != _nil) {
				node = node->right;
			}

			return node;
		} else {
			while (node == node->parent->left) {
				node = node->parent;
			}

			if (node == _root) {
				return nullptr;
			}

			return node->parent;
		}
	}

	inline void _rotate_left(Element *p_node) {
		ERR_COND_NULL(p_node);
		Element *child = p_node->right;
		p_node->right = child->left;

		if (child->left != _nil) {
			child->left->parent = p_node;
		}
		child->parent = p_node->parent;

		if (p_node == p_node->parent->left) {
			p_node->parent->left = child;
		} else {
			p_node->parent->right = child;
		}

		child->left = p_node;
		p_node->parent = child;
	}

	inline void _rotate_right(Element *p_node) {
		ERR_COND_NULL(p_node);
		Element *child = p_node->left;
		p_node->left = child->right;

		if (child->right != _nil) {
			child->right->parent = p_node;
		}
		child->parent = p_node->parent;
		if (p_node == p_node->parent->right) {
			p_node->parent->right = child;
		} else {
			p_node->parent->left = child;
		}

		child->right = p_node;
		p_node->parent = child;
	}

	void _insert_fix(Element *p_node) {
		Element *node = p_node;
		Element *parent = node->parent;
		Element *grandparent = nullptr;

		// Assume node A is red at this point
		while (parent->colour == RED) {
			grandparent = parent->parent;

			if (parent == grandparent->left) {
				Element *uncle = grandparent->right;
				if (uncle->colour == RED) {
					_set_colour(parent, BLACK);
					_set_colour(uncle, BLACK);
					_set_colour(grandparent, RED);

					node = grandparent;
					parent = node->parent;
				} else {
					if (node == parent->right) {
						_rotate_left(parent);
						node = parent;
						parent = node->parent;
					}

					_set_colour(parent, BLACK);
					_set_colour(grandparent, RED);
					_rotate_right(grandparent);
				}
			} else {
				Element *uncle = grandparent->left;
				if (uncle->colour == RED) {
					_set_colour(parent, BLACK);
					_set_colour(uncle, BLACK);
					_set_colour(grandparent, RED);
					node = grandparent;
					parent = node->parent;
				} else {
					if (node == parent->left) {
						_rotate_right(parent);
						node = parent;
						parent = node->parent;
					}

					_set_colour(parent, BLACK);
					_set_colour(grandparent, RED);
					_rotate_left(grandparent);
				}
			}
		}

		_set_colour(_root->left, BLACK);
	}

	void _insert(const T &p_value) {
		Element *parent = _root;
		Element *node = _root->left;

		while (node != _nil) {
			parent = node;
			if (p_value > node->get()) {
				node = node->right;
			} else if (p_value < parent->get()) {
				node = node->left;
			} else {
				return;
			}
		}

		Element *new_node = vnew(Element);
		new_node->parent = parent;
		new_node->left = _nil;
		new_node->right = _nil;
		new_node->_data = p_value;

		if (parent == _root || p_value < parent->_data) {
			parent->left = new_node;
		} else {
			parent->right = new_node;
		}
		new_node->_next = _successor(new_node);
		new_node->_prev = _predecessor(new_node);

		if (new_node->_next) {
			new_node->_next->_prev = new_node;
		}
		if (new_node->_prev) {
			new_node->_prev->_next = new_node;
		}

		element_count++;
		_insert_fix(new_node);
		ERR_FAIL_COND(_nil->colour != BLACK);
	}

	void _erase_fix(Element *p_node) {
		// Once again stolen from Godot
		Element *root = _root->left;
		Element *node = _nil;
		Element *parent = node->parent;
		Element *sibling = p_node;

		while (node != root) {
			if (sibling->colour == RED) {
				_set_colour(sibling, BLACK);
				_set_colour(parent, RED);
				if (sibling == parent->right) {
					sibling = sibling->left;
					_rotate_left(parent);
				} else {
					sibling = sibling->right;
					_rotate_right(parent);
				}
			}

			if ((sibling->left->colour == BLACK) && (sibling->right->colour == BLACK)) {
				_set_colour(sibling, RED);

				if (parent->colour == RED) {
					_set_colour(parent, BLACK);
					break;
				} else {
					node = parent;
					parent = node->parent;
					sibling = (node == parent->left) ? parent->right : parent->left;
				}
			} else {
				if (sibling == parent->right) {
					if (sibling->right->colour == BLACK) {
						_set_colour(sibling->left, BLACK);
						_set_colour(sibling, RED);
						_rotate_right(sibling);
						sibling = sibling->parent;
					}

					_set_colour(sibling, parent->colour);
					_set_colour(parent, BLACK);
					_set_colour(sibling->right, BLACK);
					_rotate_left(parent);
					break;
				} else {
					if (sibling->left->colour == BLACK) {
						_set_colour(sibling->right, BLACK);
						_set_colour(sibling, RED);
						_rotate_left(sibling);
						sibling = sibling->parent;
					}

					_set_colour(sibling, parent->colour);
					_set_colour(parent, BLACK);
					_set_colour(sibling->left, BLACK);
					_rotate_right(parent);
					break;
				}
			}
		}
	}

	void _erase(Element *p_node) {
		// Godot's _erase function, as is most things in this repository

		Element *replacement = ((p_node->left == _nil) || (p_node->right == _nil)) ? p_node : p_node->_next;
		Element *child = (replacement->left == _nil) ? replacement->right : replacement->left;

		Element *sibling = nullptr;
		if (replacement == replacement->parent->left) {
			replacement->parent->left = child;
			sibling = replacement->parent->right;
		} else {
			replacement->parent->right = child;
			sibling = replacement->parent->left;
		}

		if (child->colour == RED) {
			child->parent = replacement->parent;
			_set_colour(child, BLACK);
		} else if (replacement->colour == BLACK && replacement != _root) {
			_erase_fix(sibling);
		}

		if (replacement != p_node) {
			ERR_FAIL_COND(replacement == _nil);

			replacement->left = p_node->left;
			replacement->right = p_node->right;
			replacement->parent = p_node->parent;
			_set_colour(replacement, p_node->colour);

			if (p_node->left != _nil) {
				p_node->left->parent = replacement;
			}

			if (p_node->right != _nil) {
				p_node->right->parent = replacement;
			}

			if (p_node == p_node->parent->left) {
				p_node->parent->left = replacement;
			} else {
				p_node->parent->right = replacement;
			}
		}

		if (p_node->_next) {
			p_node->_next->_prev = p_node->_prev;
		}
		if (p_node->_prev) {
			p_node->_prev->_next = p_node->_next;
		}

		vdelete(p_node);
		element_count--;
		ERR_FAIL_COND(_nil->colour != BLACK);
	}

	Element *_find(const T &p_value) const {
		Element *node = _root->left;

		while (node != _nil) {
			if (p_value > node->get()) {
				node = node->right;
			} else if (p_value < node->get()) {
				node = node->left;
			} else {
				return node;
			}
		}

		return nullptr;
	}

	void _cleanup_node(Element *p_node) {
		if (p_node == _nil) {
			return;
		}

		_cleanup_node(p_node->left);
		_cleanup_node(p_node->right);
		vdelete(p_node);
	}

public:
	FORCE_INLINE uint32_t size() {
		return element_count;
	}
	FORCE_INLINE bool is_empty() {
		return element_count == 0;
	}

	FORCE_INLINE Element *find(const T &p_value) const {
		if (!_root) {
			return nullptr;
		}

		return _find(p_value);
	}

	FORCE_INLINE void insert(const T &p_value) {
		if (!_root) {
			_create_root();
		}

		_insert(p_value);
	}

	FORCE_INLINE bool erase(const T &p_value) {
		if (!_root) {
			return false;
		}

		Element *e = _find(p_value);
		if (e == nullptr) {
			return false; // Invalid key
		}

		_erase(e);
		if (element_count == 0 && _root) {
			_free_root();
		}

		return true;
	}

	void clear() {
		if (!_root) {
			return;
		}

		_cleanup_node(_root);
		_root = nullptr;
		element_count = 0;
	}

	Element *front() const {
		if (!_root) {
			return nullptr;
		}

		Element *e = _root->left;
		if (e == _nil) {
			return nullptr;
		}

		while (e->left != _nil) {
			e = e->left;
		}

		return e;
	}

	Element *back() const {
		if (!_root) {
			return nullptr;
		}

		Element *e = _root->left;
		if (e == _nil) {
			return nullptr;
		}

		while (e->right != _nil) {
			e = e->right;
		}

		return e;
	}

	void operator=(const RBSet &p_other) {
		if (this == &p_other) {
			return;
		}

		clear();
		for (Element *e = p_other.front(); e; e = e->next()) {
			insert(e->get());
		}
	}

	void operator=(RBSet &&p_other) {
		if (this == &p_other) {
			return;
		}

		SWAP(_root, p_other._root);
		SWAP(element_count, p_other.element_count);
	}

	explicit RBSet(const RBSet &p_other) {
		_create_nil();
		clear();

		for (Element *e = p_other.front(); e; e = e->next()) {
			insert(e->get());
		}
	}

	RBSet(RBSet &&p_other) {
		_create_nil();
		_root = p_other._root;
		element_count = p_other.element_count;

		p_other._root = nullptr;
		p_other.element_count = 0;
	}

	RBSet(std::initializer_list<T> p_list) {
		_create_nil();
		for (const T &e : p_list) {
			insert(e);
		}
	}

	RBSet() {
		_create_nil();
	}

	FORCE_INLINE ~RBSet() {
		clear();
		_delete_nil();
	}
};
