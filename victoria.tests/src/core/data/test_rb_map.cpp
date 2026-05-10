#include "core/data/test_rb_map.h"

#include "test_macros.h"
#include "test_manager.h"

#include <core/data/rb_map.h>

static bool rb_map_test_init_empty() {
	RBMap<bool, int> r;
	TEST_EQ(r.is_empty(), true);
	TEST_EQ(r.front(), nullptr);
	TEST_EQ(r.back(), nullptr);
	TEST_EQ(r.size(), 0);
	return true;
}

static bool rb_map_test_init_initializer() {
	RBMap<int, bool> r{{1, false}, {2, true}, {3, false}};
	TEST_EQ(r.is_empty(), false);
	TEST_NEQ(r.front(), nullptr);
	TEST_NEQ(r.back(), nullptr);
	TEST_EQ(r.size(), 3);
	return true;
}

static bool rb_map_test_init_copy_from() {
	RBMap<int, bool> r{{1, false}, {2, true}, {3, false}};
	TEST_EQ(r.size(), 3);
	RBMap<int, bool> r2(r);
	TEST_EQ(r2.size(), 3);
	TEST_NEQ(r2.front(), nullptr);
	TEST_NEQ(r2.back(), nullptr);
	TEST_EQ(r2.is_empty(), false);
	return true;
}

static bool rb_map_test_init_move() {
	RBMap<int, bool> r{{1, false}, {2, true}, {3, false}};
	TEST_EQ(r.size(), 3);
	RBMap<int, bool> r2 = std::move(r);
	TEST_EQ(r2.size(), 3);
	TEST_EQ(r.size(), 0);
	TEST_NEQ(r2.front(), nullptr);
	TEST_NEQ(r2.back(), nullptr);
	TEST_EQ(r.front(), nullptr);
	TEST_EQ(r.back(), nullptr);
	TEST_EQ(r2.is_empty(), false);
	TEST_EQ(r.is_empty(), true);
	return true;
}

static bool rb_map_test_assignment() {
	RBMap<int, bool> r{{1, false}, {2, true}, {3, false}};
	RBMap<int, bool> r2{{-1, false}, {1, true}};
	RBMap<int, bool> r3{{12, false}, {-5, true}, {-1, false}, {3, true}};
	TEST_EQ(r.size(), 3);
	TEST_EQ(r2.size(), 2);
	TEST_EQ(r3.size(), 4);
	r = r2;
	TEST_EQ(r.size(), 2);
	r2 = std::move(r3);
	TEST_EQ(r2.size(), 4);
	TEST_EQ(r3.size(), 2);
	return true;
}

static bool rb_map_test_iterator() {
	RBMap<int, bool> r{{1, false}, {2, true}, {3, false}};
	for (KeyValue<int, bool> &kv : r) {
		TEST_EQ((kv.key & 1), !kv.value);
	}

	r = {{5, true}, {-5, false}};
	for (const KeyValue<int, bool> &kv : r) {
		TEST_EQ((kv.key > 0), kv.value);
	}
	return true;
}

static bool rb_map_test_find() {
	RBMap<int, bool> r{{1, false}, {2, true}, {3, false}};
	TEST_EQ(r.find(1), r.front());
	TEST_EQ(r.find(3), r.back());
	TEST_EQ(r.find(0), nullptr);
	return true;
}

static bool rb_map_test_get() {
	RBMap<int, bool> r{{1, false}, {2, true}, {3, false}};
	TEST_EQ(r[1], false);
	TEST_EQ(r[3], false);
	TEST_EQ(r.find(1)->key(), 1);
	TEST_EQ(r.find(3)->value(), false);
	return true;
}

static bool rb_map_test_list() {
	RBMap<int, bool> r{{1, false}, {2, true}, {3, false}};
	TEST_EQ(r.find(1)->next()->value(), true);
	TEST_EQ(r.find(3)->prev()->value(), true);
	TEST_EQ(r.front()->next()->next(), r.back());
	TEST_EQ(r.front()->prev(), nullptr);
	TEST_EQ(r.back()->next(), nullptr);
	return true;
}

static bool rb_map_test_erase() {
	RBMap<int, bool> r{{1, false}, {2, true}, {3, false}};
	TEST_EQ(r.erase(2), true);
	TEST_EQ(r.size(), 2);
	TEST_EQ(r.erase(0), false);
	TEST_EQ(r.size(), 2);
	return true;
}

void rb_map_register_tests() {
	register_test(rb_map_test_init_empty, "RBMap construction with no parameters");
	register_test(rb_map_test_init_initializer, "RBMap construction using std::initializer_list");
	register_test(rb_map_test_init_copy_from, "RBMap construction by copying data from another map");
	register_test(rb_map_test_init_move, "RBMap construction by using std::move");
	register_test(rb_map_test_assignment, "RBMap assignment via copy-from and std::move");
	register_test(rb_map_test_iterator, "RBMap iteration using the Iterator and ConstIterator structs");
	register_test(rb_map_test_find, "RBMap searching and finding valid and invalid information");
	register_test(rb_map_test_get, "RBMap getting data from the [] operator and from the find function");
	register_test(rb_map_test_list, "RBMap using the linked-list format to retrieve data");
	register_test(rb_map_test_erase, "RBMap erasing data depending on whether it exists in the map");
}