#include "core/data/test_list.h"

#include "test_macros.h"
#include "test_manager.h"

#include <core/data/list.h>

static bool list_test_init_empty() {
	List<int> l;
	TEST_EQ(l.size(), 0);
	TEST_EQ(l.is_empty(), true);
	return true;
}

static bool list_test_init_initializer() {
	List<int> l{1, 2, 3, 4, 5, 6};
	TEST_EQ(l.size(), 6);
	l = {7, 8, 9, 10};
	TEST_EQ(l.size(), 4);
	return true;
}

static bool list_test_init_copy_from() {
	List<int> l1{0, 1, 2, 3};
	TEST_EQ(l1.size(), 4);
	List<int> l2 = List<int>(l1);
	TEST_EQ(l2.size(), 4);
	List<int> l3(l1);
	TEST_EQ(l3.size(), 4);
	return true;
}

static bool list_test_init_move() {
	List<int> l{0, 1, 2, 3};
	TEST_EQ(l.size(), 4);
	List<int> l2 = std::move(l);
	TEST_EQ(l2.size(), 4);
	TEST_EQ(l.size(), 0);
	List<int> l3(std::move(l2));
	TEST_EQ(l3.size(), 4);
	TEST_EQ(l2.size(), 0);
	return true;
}

static bool list_test_assignment_other() {
	List<int> l{0, 1, 2, 3};
	List<int> l2{4, 5, 6, 7};
	TEST_EQ(l.size(), 4);
	TEST_EQ(l2.size(), 4);
	l = l2;
	List<int>::Element *e1 = l.front();
	List<int>::Element *e2 = l2.front();
	TEST_NEQ(e1, nullptr);
	TEST_NEQ(e2, nullptr);
	TEST_EQ(e1->get(), e2->get());
	return true;
}

static bool list_test_assignment_move() {
	List<int> l1{0, 1, 2, 3};
	List<int> l2{4, 5, 6, 7};
	TEST_EQ(l1.size(), 4);
	TEST_EQ(l2.size(), 4);
	l1 = std::move(l2);
	TEST_EQ(l1.size(), 4);
	TEST_EQ(l2.size(), 0);
	TEST_EQ(l1.front()->get(), 4);
	return true;
}

static bool list_test_assignment_initializer() {
	List<int> l{0, 1, 2, 3};
	TEST_EQ(l.size(), 4);
	l = {4, 5, 6};
	TEST_EQ(l.size(), 3);
	TEST_EQ(l.front()->get(), 4);
	return true;
}

static bool list_test_get() {
	List<int> l{0, 1, 2, 3};
	int &v = l.get(1);
	TEST_EQ(v, 1);
	const int &v2 = l.get(1);
	TEST_EQ(v2, 1);
	v = 7;
	TEST_EQ(v2, 7);
	TEST_EQ(v2, l.get(1));
	return true;
}

static bool list_test_clear() {
	List<int> l{0, 1, 2, 3};
	TEST_EQ(l.size(), 4);
	l.clear();
	TEST_EQ(l.size(), 0);
	return true;
}

static bool list_test_iterator() {
	List<int> l{0, 1, 2, 3};
	int i = 0;
	for (int &e : l) {
		TEST_EQ(e, i);
		e += 2;
		i++;
	}

	i = 2;
	for (const int &e : l) {
		TEST_EQ(e, i);
		i++;
	}
	return true;
}

static bool list_test_search() {
	List<int> l{0, 1, 2, 3};
	List<int>::Element *e = l.find(2);
	TEST_NEQ(e, nullptr);
	TEST_EQ(e->get(), 2);
	TEST_EQ(e->next(), l.back());
	e = l.find(12);
	TEST_EQ(e, nullptr);
	return true;
}

static bool list_test_has() {
	List<int> l{0, 2, 4, 6};
	TEST_EQ(l.has(0), true);
	TEST_EQ(l.has(4), true);
	TEST_EQ(l.has(5), false);
	TEST_EQ(l.has(INT16_MAX), false);
	return true;
}

static bool list_test_append() {
	List<int> l{0, 1, 2, 3};
	TEST_EQ(l.size(), 4);
	List<int>::Element *e = l.push_back(4);
	TEST_NEQ(e, nullptr);
	TEST_EQ(e, l.back());
	TEST_EQ(e->get(), 4);
	e = l.push_back(5);
	TEST_NEQ(e, nullptr);
	TEST_EQ(e, l.back());
	TEST_EQ(e->get(), 5);
	return true;
}

static bool list_test_prepend() {
	List<int> l{0, 1, 2, 3};
	TEST_EQ(l.size(), 4);
	List<int>::Element *e = l.push_front(1);
	TEST_NEQ(e, nullptr);
	TEST_EQ(e, l.front());
	TEST_EQ(e->get(), 1);
	e = l.push_front(2);
	TEST_NEQ(e, nullptr);
	TEST_EQ(e, l.front());
	TEST_EQ(e->get(), 2);
	return true;
}

static bool list_test_erase() {
	List<int> l{0, 1, 2, 3};
	TEST_EQ(l.size(), 4);
	l.erase(1);
	TEST_EQ(l.size(), 3);
	List<int>::Element *e = l.front();
	TEST_NEQ(e, nullptr);
	TEST_EQ(e->get(), 0);
	l.erase(e);
	TEST_EQ(l.size(), 2);
	return true;
}

static bool list_test_pop() {
	List<int> l{0, 1, 2, 3};
	TEST_EQ(l.size(), 4);
	l.pop_back();
	TEST_EQ(l.size(), 3);
	l.pop_front();
	TEST_EQ(l.size(), 2);
	return true;
}

void list_register_tests() {
	register_test(list_test_init_empty, "List initialization with no parameters");
	register_test(list_test_init_initializer, "List initialization from an std::initializer_list");
	register_test(list_test_init_copy_from, "List initialization by copying data from another list");
	register_test(list_test_init_move, "List initialization by moving a list in with std::move");
	register_test(list_test_assignment_other, "List assignment from another list");
	register_test(list_test_assignment_move, "List assignment from moving a list in with std::move");
	register_test(list_test_assignment_initializer, "List assignment from an std::initializer_list");
	register_test(list_test_get, "List obtaining values via the get method and ensuring references are consistent");
	register_test(list_test_clear, "List clearing by deleting all elements remaining in the list");
	register_test(list_test_iterator, "List iterators with constant and non-constant values");
	register_test(list_test_search, "List searching for elements that do and do not exist in the list");
	register_test(list_test_has, "List checks to see if given elements exist in the list");
	register_test(list_test_append, "Appending elements to the back of the list");
	register_test(list_test_prepend, "Appending elements to the front of the list");
	register_test(list_test_erase, "List erasing elements from said list");
	register_test(list_test_pop, "List popping elements from the front and back of the list");
}
