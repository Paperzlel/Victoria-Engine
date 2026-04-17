#include "core/data/test_hashtable.h"

#include "test_macros.h"
#include "test_manager.h"

#include <core/data/hashtable.h>

struct hashstruct1 {
	bool used;
	int i;
};

// Initializer/assignment tests
// We test for assignment operators where possible simply because their constructor is identical bar a handful of
// guards.

static bool hashtable_test_init_empty() {
	HashTable<int, int> h;
	TEST_EQ(h.size(), 0);
	TEST_EQ(h.is_empty(), true);
	return true;
}

static bool hashtable_test_init_capacity() {
	HashTable<int, int> h(12);
	TEST_EQ(h.size(), 0);
	TEST_EQ(h.get_capacity(), 29); // this is the next capacity that 12 fits into
	return true;
}

static bool hashtable_test_init_initializer() {
	HashTable<int, int> h{{1, 1}, {2, 2}, {3, 3}};
	TEST_EQ(h.size(), 3);
	h = {{4, 4}, {5, 5}};
	TEST_EQ(h.size(), 2);
	return true;
}

static bool hashtable_test_init_copy_from() {
	HashTable<int, bool> h = {{0, true}, {1, false}};
	TEST_EQ(h.size(), 2);
	HashTable<int, bool> h2 = HashTable<int, bool>(h); // Require explicit creation
	TEST_EQ(h2.size(), 2);
	HashTable<int, bool> h3(h2);
	TEST_EQ(h3.size(), 2);
	return true;
}

static bool hashtable_test_move() {
	HashTable<int, bool> h = {{0, true}, {1, false}, {2, true}};
	TEST_EQ(h.size(), 3);
	HashTable<int, bool> h2 = std::move(h);
	TEST_EQ(h2.size(), 3);
	TEST_EQ(h.size(), 0);
	HashTable<int, bool> h3(std::move(h2));
	TEST_EQ(h3.size(), 3);
	TEST_EQ(h2.size(), 0);
	return true;
}

// Test for if referenced data is actually mutable and consistent without hashing.
static bool hashtable_test_get() {
	HashTable<int, bool> h = {{0, true}, {1, false}, {2, false}};
	bool &value = h.get(1);
	TEST_EQ(value, false);
	const bool &value2 = h.get(1);
	TEST_EQ(value2, false);
	value = true;
	TEST_EQ(value, value2);
	TEST_EQ(value2, h.get(1));
	return true;
}

// Test for if pointers to data are the same for const and non-const pointers.
static bool hashtable_test_get_ptr() {
	HashTable<int, bool> h = {{0, true}, {1, false}, {2, false}, {3, true}};
	bool *value = h.get_ptr(0);
	TEST_NEQ(value, nullptr);
	TEST_EQ(*value, true);
	TEST_EQ(value, h.get_ptr(0));
	const bool *value2 = h.get_ptr(0);
	TEST_NEQ(value2, nullptr);
	TEST_EQ(value2, value);
	*value = false;
	TEST_EQ(*value2, false);
	TEST_EQ(nullptr, h.get_ptr(4));
	return true;
}

// Test if operator[] returns the correct data, and if that data is properly referenced.
static bool hashtable_test_bracket_operator() {
	HashTable<int, int> h = {{0, 0}, {1, 1}, {2, 2}, {3, 3}};
	int &x = h[2];
	TEST_EQ(x, 2);
	const int &x2 = h[2];
	TEST_EQ(x, x2);
	x = 4;
	TEST_EQ(x, x2);
	TEST_EQ(x, h[2]);
	return true;
}

// Test if our Iterator and ConstIterator struct function correctly.
static bool hashtable_test_iterator() {
	HashTable<int, int> h = {{0, 0}, {1, 1}, {2, 2}, {3, 3}};
	for (KeyValue<int, int> &kv : h) {
		TEST_EQ(kv.key, kv.value);
		kv.value += 2;
	}

	for (const KeyValue<int, int> &kv : h) {
		TEST_EQ(kv.key + 2, kv.value);
	}
	return true;
}

// Test if the `has()` function works correctly.
static bool hashtable_test_has() {
	HashTable<int, bool> h = {{0, true}, {2, false}, {4, false}, {6, true}};
	TEST_EQ(h.has(0), true);
	TEST_EQ(h.has(6), true);
	TEST_EQ(h.has(10), false);
	TEST_EQ(h.has(INT32_MAX), false);
	return true;
}

// Test if the `insert()` function works on pre-existing data as well as initializing data.
static bool hashtable_test_insert() {
	HashTable<int, bool> h = {{0, true}, {1, false}, {2, false}, {3, true}};
	h.insert(6, false);
	TEST_EQ(h.size(), 5);
	TEST_EQ(h.get(6), false);
	HashTable<int, bool> h2;
	h2.insert(1, false);
	TEST_EQ(h2.size(), 1);
	TEST_EQ(h2.get_capacity(), 5);
	TEST_EQ(h2.get(1), false);
	return true;
}

// Test if `operator[]` actually inserts data as expected by user code.
static bool hashtable_test_bracket_insert() {
	HashTable<int, bool> h = {{0, true}, {1, false}, {2, false}, {3, true}};
	h[6] = false;
	TEST_EQ(h.size(), 5);
	TEST_EQ(h[6], false);
	HashTable<int, bool> h2;
	h2[12] = true;
	TEST_EQ(h2.size(), 1);
	TEST_EQ(h2[12], true);
	return true;
}

// Test if references to data remain true after rehashes.
static bool hashtable_test_reference_info() {
	HashTable<int, bool> h = {{0, true}, {1, false}, {2, false}, {3, true}};
	bool &value = h[0];
	TEST_EQ(value, true);
	h[6] = true;
	TEST_EQ(h.size(), 5);
	TEST_EQ(h[6], true);
	TEST_EQ(value, true);
	value = false;
	TEST_EQ(h[0], false);
	return true;
}

// Test if pointers to data remain consistent after rehashes.
static bool hashtable_test_pointer_info() {
	HashTable<int, bool> h = {{0, true}, {1, false}, {2, false}, {3, true}};
	bool *value = h.get_ptr(1);
	TEST_NEQ(value, nullptr);
	h[14] = false;
	const bool *value2 = h.get_ptr(1);
	TEST_NEQ(value2, nullptr);
	TEST_EQ(value2, value);
	*value = true;
	TEST_EQ(h.get(1), true);
	return true;
}

// Test if data is properly erased when requested.
static bool hashtable_test_erase() {
	HashTable<int, bool> h = {{0, true}, {1, false}, {2, false}, {3, true}};
	h.erase(0);
	TEST_EQ(h.has(0), false);
	TEST_EQ(h.size(), 3);
	return true;
}

// Test if erasing data below a hash boundary and then adding a new element retains the capacity without reallocating.
static bool hashtable_test_erase_rehash() {
	HashTable<int, bool> h = {{0, true}, {1, false}, {2, false}, {3, true}};
	h[4] = false;
	h.erase(2);
	TEST_EQ(h.size(), 4);
	TEST_EQ(h.get_capacity(), 11);
	h[5] = false;
	TEST_EQ(h.size(), 5);
	TEST_EQ(h.get_capacity(), 11);
	return true;
}

void hashtable_register_tests() {
	register_test(hashtable_test_init_empty, "Hashtable construction with no parameters");
	register_test(hashtable_test_init_capacity, "Hashtable creation with a predetermined capacity");
	register_test(hashtable_test_init_initializer, "Hashtable creation and assignment using std::initializer_list");
	register_test(hashtable_test_init_copy_from, "Hashtable contruction with copy-from assignment");
	register_test(hashtable_test_move, "Hashtable construction from std::move operations");
	register_test(hashtable_test_get, "Hashtable get function using regular and constant values");
	register_test(hashtable_test_get_ptr, "Hashtable get_ptr function using regular and constant values");
	register_test(hashtable_test_bracket_operator, "Hashtable reading and modifying data using the [] operator");
	register_test(hashtable_test_iterator, "Hashtable reading using constant and non-constant iterators");
	register_test(hashtable_test_has, "Hashtable entry checking using the has function");
	register_test(hashtable_test_insert, "Hashtable insertion from empty and with existing elements");
	register_test(hashtable_test_bracket_insert, "Hashtable insertion using the [] operator");
	register_test(hashtable_test_reference_info, "Hashtable reading and writing after a rehash using references");
	register_test(hashtable_test_pointer_info, "Hashtable reading and writing after a reshash using pointers");
	register_test(hashtable_test_erase, "Hashtable erasing individual information");
	register_test(hashtable_test_erase_rehash, "Hashtable erasing and re-inserting over a remap boundary");
}
