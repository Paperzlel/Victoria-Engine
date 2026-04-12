#include "test_hashtable.h"
#include "test_macros.h"
#include "test_manager.h"

#include <core/data/hashtable.h>

struct hashstruct1 {
	bool used;
	int i;
};

static bool hashtable_test_basic() {
	HashTable<String, int> h;
	h["a"] = 1;
	TEST_EQ(h["a"], 1);

	const int ret1 = h["a"];
	TEST_EQ(ret1, 1);
	TEST_EQ(h.erase("a"), true);
	TEST_EQ(h.is_empty(), true);

	for (int i = 0; i < 12; i++) {
		String hash = "test_" + String((char)(i + 48));
		h[hash] = i;
	}

	return true;
}

static bool hashtable_test_struct() {
	HashTable<String, hashstruct1> h;
	hashstruct1 a;
	a.i = 3;
	a.used = true;
	h["a"] = a;

	hashstruct1 b = h["a"];
	TEST_EQ(h.get_element_count(), 1);
	TEST_EQ(a.i, b.i);
	TEST_EQ(a.used, b.used);
	TEST_EQ(h.erase("a"), true);
	TEST_EQ(h.get_element_count(), 0);

	return true;
}

static bool hashtable_test_hashing() {
	uint64_t hash1 = HasherDefault::hash("b");
	uint64_t hash2 = HasherDefault::hash("b");
	TEST_EQ(hash1, hash2);

	uint64_t hash3 = HasherDefault::hash("B");
	TEST_NEQ(hash1, hash3);

	uint64_t hash4 = HasherDefault::hash("abcdef");
	uint64_t hash5 = HasherDefault::hash("fedcba");
	TEST_NEQ(hash4, hash5);

	return true;
}

void hashtable_register_tests() {
	register_test(hashtable_test_basic, "Hashtable reading and writing with atomic data");
	register_test(hashtable_test_struct, "Hashtable reading and writing with a struct");
	register_test(hashtable_test_hashing, "Hashing results with similar inputs");
}
