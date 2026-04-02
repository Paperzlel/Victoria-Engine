#include "test_list.h"
#include "test_macros.h"
#include "test_manager.h"

#include <core/data/list.h>

struct liststruct1 {
	int a;
	bool b;
};

static bool list_test_basic() {
	List<int> l;
	l.push_back(1);
	TEST_EQ(l.front()->get(), 1);

	l.clear();
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

	l.clear();
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
	l.clear();
	return true;
}

void list_register_tests() {
	register_test(list_test_basic, "List reading and writing with atomic data");
	register_test(list_test_struct, "List reading and writing with structures");
	register_test(list_test_length, "List writing to a specific size and clearing itself");
}
