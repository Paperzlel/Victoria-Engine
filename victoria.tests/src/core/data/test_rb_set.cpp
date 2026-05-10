#include "core/data/test_rb_set.h"

#include "test_macros.h"
#include "test_manager.h"

#include <core/data/rb_set.h>

static bool rb_set_test_init_empty() {
	RBSet<int> r;
	TEST_EQ(r.is_empty(), true);
	TEST_EQ(r.front(), nullptr);
	TEST_EQ(r.back(), nullptr);
	TEST_EQ(r.size(), 0);
	return true;
}

static bool rb_set_test_init_initializer_list() {
	RBSet<int> r{1, 2, 3, 4, 5};
	TEST_EQ(r.is_empty(), false);
	TEST_NEQ(r.front(), nullptr);
	TEST_NEQ(r.back(), nullptr);
	TEST_EQ(r.size(), 5);
	return true;
}

static bool rb_set_test_init_copy_from() {
	RBSet<int> r{1, 2, 3, 4, 5};
	TEST_EQ(r.size(), 5);
	RBSet<int> r2(r);
	TEST_EQ(r2.size(), 5);
	TEST_NEQ(r2.front(), nullptr);
	TEST_NEQ(r2.back(), nullptr);
	TEST_EQ(r2.is_empty(), false);
	return true;
}

static bool rb_set_test_init_move() {
	RBSet<int> r{1, 2, 3, 4, 5};
	TEST_EQ(r.size(), 5);
	RBSet<int> r2 = std::move(r);
	TEST_EQ(r2.size(), 5);
	TEST_EQ(r.size(), 0);
	TEST_NEQ(r2.front(), nullptr); // TODO: _nil needs to be re-assigned to each node in the tree on move.
	TEST_NEQ(r2.back(), nullptr);
	TEST_EQ(r.front(), nullptr);
	TEST_EQ(r.back(), nullptr);
	TEST_EQ(r.is_empty(), true);
	return true;
}

static bool rb_set_test_assignment() {
	RBSet<int> r{1, 2, 3, 4, 5};
	RBSet<int> r2{2, 3, 4};
	RBSet<int> r3{6, 7, 8, 9};
	TEST_EQ(r.size(), 5);
	TEST_EQ(r2.size(), 3);
	r = r2;
	TEST_EQ(r.size(), 3);
	r2 = std::move(r3);
	TEST_EQ(r2.size(), 4);
	TEST_EQ(r3.size(), 3);
	return true;
}

static bool rb_set_test_iterator() {
	RBSet<int> r{1, 2, 3, 4, 5};
	int i = 1;
	for (int &v : r) {
		TEST_EQ(v, i);
		i++;
	}

	r = {6, 7, 8, 9};
	i = 6;
	for (const int &v : r) {
		TEST_EQ(v, i);
		i++;
	}
	return true;
}

static bool rb_set_test_find() {
	RBSet<int> r{1, 2, 3, 4, 5};
	TEST_EQ(r.find(5), r.back());
	TEST_EQ(r.find(1), r.front());
	TEST_EQ(r.find(12), nullptr);
	return true;
}

static bool rb_set_test_list_get() {
	RBSet<int> r{1, 2, 3, 4, 5};
	TEST_EQ(r.find(2)->get(), 2);
	TEST_EQ(r.find(3)->prev()->get(), 2);
	TEST_EQ(r.find(5)->next(), nullptr);
	return true;
}

static bool rb_set_test_erase() {
	RBSet<int> r{1, 2, 3, 4, 5};
	TEST_EQ(r.erase(3), true);
	TEST_EQ(r.size(), 4);
	TEST_EQ(r.erase(10), false);
	TEST_EQ(r.size(), 4);
	return true;
}

void rb_set_register_tests() {
	register_test(rb_set_test_init_empty, "RBSet construction with no initial data");
	register_test(rb_set_test_init_initializer_list, "RBSet construction from an std::initializer_list");
	register_test(rb_set_test_init_copy_from, "RBSet construction from another class");
	register_test(rb_set_test_init_move, "RBSet construction from an std::move operation");
	register_test(rb_set_test_assignment, "RBSet assignment operators with a copy-from and move operation");
	register_test(rb_set_test_iterator, "RBSet Iterator and ConstIterator structures");
	register_test(rb_set_test_find, "RBSet find functionality");
	register_test(rb_set_test_list_get, "RBSet getting items from its list-like structure");
	register_test(rb_set_test_erase, "RBSet erasing data from the list");
}
