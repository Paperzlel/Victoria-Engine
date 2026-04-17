#include "core/data/test_list.h"

#include "test_macros.h"
#include "test_manager.h"

#include <core/data/list.h>

struct liststruct1 {
	int a;
	bool b;
};

static bool list_test_initializer() {
	List<int> l{1, 2, 3, 4, 5, 6};
	TEST_EQ(l.size(), 6);
	TEST_EQ(l.front()->get(), 1);
	TEST_EQ(l.back()->get(), 6);

	l = {7, 8, 9, 10};
	TEST_EQ(l.size(), 4);
	TEST_EQ(l.front()->get(), 7);
	TEST_EQ(l.back()->get(), 10);
	return true;
}

static bool list_test_basic() {
	List<int> l;
	l.push_back(1);
	TEST_EQ(l.front()->get(), 1);

	return true;
}

static bool list_test_struct() {
	List<liststruct1> l;
	liststruct1 a;
	a.a = 12;
	a.b = true;
	l.push_back(a);
	TEST_EQ(l.front()->get().a, 12);
	TEST_EQ(l.front()->get().b, true);

	return true;
}

static bool list_test_length() {
	List<int> l;

	for (int i = 0; i < 256; i++) {
		l.push_back(i);
	}

	TEST_EQ(l.size(), 256);

	for (int j = 255; j >= 0; j--) {
		TEST_EQ(l.erase(j), true);
	}

	TEST_EQ(l.size(), 0);
	return true;
}

static bool list_test_foreach() {
	List<bool> l{true, true, true};
	for (bool b : l) {
		TEST_EQ(b, true);
	}

	List<liststruct1> l2 = {{12, true}, {1, true}};
	for (const liststruct1 &i : l2) {
		TEST_EQ(i.b, true);
	}

	return true;
}

static bool list_test_references() {
	List<uint32_t> l{20, 19, 18, 17, 16, 15, 14, 13};
	TEST_EQ(l.size(), 8);
	uint32_t &ref = l.get(3);
	ref = 12;
	TEST_EQ(l.get(3), 12);
	const uint32_t &ref2 = l.get(3);
	TEST_EQ(ref2, 12);
	ref = 17;
	TEST_EQ(ref2, 17);
	l.pop_back();

	return true;
}

static bool list_test_move() {
	List<int64_t> l{-1, 0, 5, 2};
	TEST_EQ(l.size(), 4);
	List<int64_t> l2 = std::move(l);
	TEST_EQ(l.size(), 0);
	TEST_EQ(l2.size(), 4);
	List<int64_t> l3(std::move(l2));
	TEST_EQ(l2.size(), 0);
	TEST_EQ(l3.size(), 4);

	return true;
}

static bool list_test_element_pointer() {
	List<bool> l = {false, true, false, true};
	TEST_EQ(l.size(), 4);
	List<bool>::Element *e = l.front();
	TEST_EQ(e->get(), false);
	e = e->next();
	TEST_EQ(e->get(), true);
	l.pop_front();
	TEST_EQ(e->get(), true);
	TEST_EQ(e->prev(), nullptr);
	TEST_EQ(e, l.front());

	return true;
}

void list_register_tests() {
	register_test(list_test_initializer, "List initializing and reading from std::initializer_list");
	register_test(list_test_basic, "List reading and writing with atomic data");
	register_test(list_test_struct, "List reading and writing with structures");
	register_test(list_test_length, "List writing to a specific size and clearing itself");
	register_test(list_test_foreach, "List reading from a foreach loop");
	register_test(list_test_references, "List writing and modifying its data with references");
	register_test(list_test_move, "List moving data into a structure via std::move");
	register_test(list_test_element_pointer, "List testing pointer consistency when modifying data");
}
