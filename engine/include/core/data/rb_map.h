#pragma once

#include "key_value.h"

#include "core/error/error_macros.h"
#include "core/os/memory.h"
#include "core/typedefs.h"

/**
 * Implementation of a RB Map (see https://en.wikipedia.org/wiki/Red%E2%80%93black_tree)
 */

template <typename K, typename V>
class RBMap {
	enum Colour {
		RED,
		BLACK
	};

public:
	class Element {
	private:
		friend class RBMap<K, V>;
		int colour = RED;
		Element *left = nullptr;
		Element *right = nullptr;
		Element *parent = nullptr;
		Element *_next = nullptr;
		Element *_prev = nullptr;
		KeyValue<K, V> _data;

	public:
		KeyValue<K, V> &key_value() {
			return _data;
		}

		const KeyValue<K, V> &key_value() const {
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

		const K &key() const {
			return _data.key;
		}

		V &value() {
			return _data.value;
		}

		const V &value() const {
			return _data.value;
		}

		FORCE_INLINE Element(const KeyValue<K, V> &p_kv) :
			_data(p_kv) {}
	};

	struct Iterator {
		friend class RBMap<K, V>;

		FORCE_INLINE KeyValue<K, V> &operator*() const {
			return e->key_value();
		}
		FORCE_INLINE KeyValue<K, V> *operator->() const {
			return &e->key_value();
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

private:
	Element *_root = nullptr;
	// An element that has all of its data uninitialized. Has no parent or sub-nodes or keys, and is set to black as
	// its default colour.
	Element *_nil = nullptr;
	u32 element_count = 0;

	void _create_root() {
		_root = vnew(Element(KeyValue<K, V>(K(), V())));
		_root->parent = _root->left = _root->right = _nil;
		_root->colour = BLACK;
	}

	void _free_root() {
		if (_root) {
			vdelete(_root);
			_root = nullptr;
		}
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
		ERR_COND_NULL(p_node);
		Element *node = p_node;
		Element *parent = node->parent;

		// Assume node A is red at this point

		while (parent != _nil) {
			if (parent->colour == BLACK) {
				return;
			}

			Element *grandparent = parent->parent;
			if (!grandparent) {
				parent->colour = BLACK;
				return;
			}

			if (parent == grandparent->left) {
				Element *uncle = grandparent->right;
				if (uncle->colour == RED) {
					parent->colour = BLACK;
					uncle->colour = BLACK;
					grandparent->colour = RED;

					node = grandparent;
					parent = node->parent;
				} else {
					if (node == parent->right) {
						_rotate_left(parent);
						node = parent;
						parent = node->parent;
					}

					parent->colour = BLACK;
					grandparent->colour = RED;
					_rotate_right(parent);
				}
			} else {
				Element *uncle = grandparent->left;
				if (uncle->colour == RED) {
					parent->colour = BLACK;
					uncle->colour = BLACK;
					grandparent->colour = RED;
					node = grandparent;
					parent = node->parent;
				} else {
					if (node == parent->left) {
						_rotate_right(parent);
						node = parent;
						parent = node->parent;
					}

					parent->colour = BLACK;
					grandparent->colour = RED;
					_rotate_left(parent);
				}
			}
		}

		_root->left->colour = BLACK;
	}

	void _insert(const K &p_key, const V &p_value) {
		Element *parent = _root;
		Element *node = _root->left;

		while (node != _nil) {
			parent = node;
			if (p_key > node->key()) {
				node = node->right;
			} else if (p_key < parent->key()) {
				node = node->left;
			} else {
				node->_data.value = p_value; // Override the currently held value
				return;
			}
		}

		Element *new_node = vnew(Element(KeyValue<K, V>(p_key, p_value)));
		new_node->parent = parent;
		new_node->left = _nil;
		new_node->right = _nil;

		if (parent == _root || p_key < parent->_data.key) {
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
		_insert_fix(new_node);

		element_count++;
		ERR_FAIL_COND(_nil->colour != BLACK);
	}

	void _erase_fix(Element *p_node) {
		// Once again stolen from Godot

		ERR_COND_NULL(p_node);
		Element *node = _nil;
		Element *parent = node->parent;
		Element *sibling = p_node;

		while (node != _root) {
			if (sibling->colour == RED) {
				sibling->colour = BLACK;
				parent->colour = RED;
				if (sibling == parent->right) {
					sibling = sibling->left;
					_rotate_left(parent);
				} else {
					sibling = sibling->right;
					_rotate_right(parent);
				}
			}

			if ((sibling->left->colour == BLACK) && (sibling->right->colour == BLACK)) {
				sibling->colour = RED;

				if (parent->colour == RED) {
					parent->colour = BLACK;
					break;
				} else {
					node = parent;
					parent = node->parent;
					sibling = (node == parent->left) ? parent->right : parent->left;
				}
			} else {
				if (sibling == parent->right) {
					if (sibling->right->colour == BLACK) {
						sibling->left->colour = BLACK;
						sibling->colour = RED;
						_rotate_right(sibling);
						sibling = sibling->parent;
					}

					sibling->colour = parent->colour;
					parent->colour = BLACK;
					sibling->right->colour = BLACK;
					_rotate_left(parent);
					break;
				} else {
					if (sibling->left->colour == BLACK) {
						sibling->right->colour = BLACK;
						sibling->colour = RED;
						_rotate_left(sibling);
						sibling = sibling->parent;
					}

					sibling->colour = parent->colour;
					parent->colour = BLACK;
					sibling->left->colour = BLACK;
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
			child->colour = BLACK;
		} else if (replacement->colour == BLACK && replacement != _root) {
			_erase_fix(sibling);
		}

		if (replacement != p_node) {
			ERR_FAIL_COND(replacement == _nil);

			replacement->left = p_node->left;
			replacement->right = p_node->right;
			replacement->parent = p_node->parent;
			replacement->colour = p_node->colour;

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

	Element *_find(const K &p_key) const {
		Element *node = _root->left;

		while (node != _nil) {
			if (p_key > node->key()) {
				node = node->right;
			} else if (p_key < node->key()) {
				node = node->left;
			} else {
				return node;
			}
		}

		return _nil;
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
	FORCE_INLINE u32 size() {
		return element_count;
	}
	FORCE_INLINE bool is_empty() {
		return element_count == 0;
	}

	FORCE_INLINE Element *find(const K &p_key) const {
		if (!_root) {
			return nullptr;
		}

		return _find(p_key);
	}

	FORCE_INLINE void insert(const K &p_key, const V &p_value) {
		if (!_root) {
			_create_root();
		}

		_insert(p_key, p_value);
	}

	FORCE_INLINE bool erase(const K &p_key) {
		if (!_root) {
			return false;
		}

		Element *e = _find(p_key);
		if (e == _nil) {
			return false; // Invalid key
		}

		_erase(e);
	}

	void clear() {
		if (!_root) {
			return;
		}

		_cleanup_node(_root);
		_root = nullptr;
		element_count = 0;
	}

	V &operator[](const K &p_key) {
		Element *e = _find(p_key);
		ERR_COND_FATAL(e == _nil);
		return e->_data.value;
	}

	const V &operator[](const K &p_key) const {
		Element *e = _find(p_key);
		ERR_COND_FATAL(e == _nil);
		return e->_data.value;
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

	FORCE_INLINE RBMap() {
		_nil = vnew(Element(KeyValue<K, V>(K(), V())));
		_nil->parent = _nil->left = _nil->right = _nil;
		_nil->colour = BLACK;
	}

	FORCE_INLINE ~RBMap() {
		clear();
		vdelete(_nil);
	}
};
