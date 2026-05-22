#include "core/data/test_vector.h"

#include "test_macros.h"
#include "test_manager.h"

#include <core/data/vector.h>

static bool vector_test_init_empty() {
	Vector<int> v;
	TEST_EQ(v.is_empty(), true);
	TEST_EQ(v.size(), 0);
	TEST_EQ(v.get_reference_count(), 0);
	return true;
}

static bool vector_test_init_list() {
	Vector<int> v{1, 2, 3, 4, 5};
	TEST_EQ(v.is_empty(), false);
	TEST_EQ(v.size(), 5);
	TEST_EQ(v.get_reference_count(), 1);
	return true;
}

static bool vector_test_init_copy_from() {
	Vector<int> v{1, 2, 3, 4, 5};
	TEST_EQ(v.size(), 5);
	TEST_EQ(v.get_reference_count(), 1);
	Vector<int> v2(v);
	TEST_EQ(v2.size(), 5);
	TEST_EQ(v2.get_reference_count(), v.get_reference_count());
	return true;
}

static bool vector_test_init_move() {
	Vector<int> v{1, 2, 3, 4, 5};
	TEST_EQ(v.size(), 5);
	TEST_EQ(v.get_reference_count(), 1);
	Vector<int> v2(std::move(v));
	TEST_EQ(v2.size(), 5);
	TEST_EQ(v2.get_reference_count(), 1);
	TEST_EQ(v2.get_reference_count(), 1);
	return true;
}

static bool vector_test_assignment() {
	Vector<int> v{1, 2, 3, 4, 5};
	Vector<int> v2{6, 7};
	Vector<int> v3{0};
	Vector<int> v4{-1, -2, -3};
	TEST_EQ(v2.size(), 2);
	TEST_EQ(v3.size(), 1);
	TEST_EQ(v4.size(), 3);
	v2 = v;
	TEST_EQ(v2.size(), 5);
	TEST_EQ(v2.get_reference_count(), 2);
	v3 = std::move(v2);
	TEST_EQ(v3.size(), 5);
	TEST_EQ(v2.get_reference_count(), 0);
	v4 = {4, 8, 12, 16, 20};
	TEST_EQ(v4.size(), 5);
	TEST_EQ(v4.get_reference_count(), 1);
	return true;
}

static bool vector_test_iterator() {
	Vector<int> v{1, 2, 3, 4, 5};
	int a = 1;
	for (const int &i : v) {
		TEST_EQ(i, a);
		a++;
	}

	a = 0;
	for (int &i : v) {
		i = a;
		TEST_EQ(v[a], a);
		a++;
	}

	return true;
}

static bool vector_test_get() {
	Vector<int> v{1, 2, 3, 4, 5};
	TEST_EQ(v.get(0), 1);
	TEST_EQ(v.get(4), 5);
	TEST_EQ(v[2], 3);
	TEST_EQ(v[3], 4);
	return true;
}

static bool vector_test_set() {
	Vector<int> v{1, 2, 3, 4, 5};
	v.set(2, 2);
	TEST_EQ(v[2], 2);
	v[3] = 2;
	TEST_EQ(v[3], 2);
	return true;
}

static bool vector_test_find() {
	Vector<int> v{1, 2, 3, 4, 5};
	TEST_EQ(v.find(2), 1);
	TEST_EQ(v.find(5), 4);
	TEST_EQ(v.find(132134), -1);
	return true;
}

static bool vector_test_has() {
	Vector<int> v{1, 2, 3, 4, 5};
	TEST_EQ(v.has(2), true);
	TEST_EQ(v.has(0), false);
	TEST_EQ(v.has(5), true);
	return true;
}

static bool vector_test_push() {
	Vector<int> v{1, 2, 3, 4, 5};
	TEST_EQ(v.size(), 5);
	v.push_back(6);
	TEST_EQ(v.size(), 6);
	TEST_EQ(v[0], 1);
	TEST_EQ(v[5], 6);
	Vector<int> v2{7, 8, 9, 10};
	v.append_array(v2);
	TEST_EQ(v.size(), 10);
	TEST_EQ(v[9], 10);
	return true;
}

static bool vector_test_insert() {
	Vector<int> v{1, 2, 3, 4, 5};
	TEST_EQ(v.size(), 5);
	v.insert(3, 4);
	TEST_EQ(v.size(), 6);
	TEST_EQ(v[3], 4);
	TEST_EQ(v[4], 3);
	TEST_EQ(v[5], 5);
	return true;
}

static bool vector_test_erase() {
	Vector<int> v{1, 2, 3, 4, 5};
	TEST_EQ(v.size(), 5);
	v.remove_at(0);
	TEST_EQ(v.size(), 4);
	TEST_EQ(v[0], 2);
	return true;
}

static bool vector_test_clear() {
	Vector<int> v{1, 2, 3, 4, 5};
	TEST_EQ(v.size(), 5);
	TEST_EQ(v.is_empty(), false);
	v.clear();
	TEST_EQ(v.size(), 0);
	TEST_EQ(v.is_empty(), true);
	return true;
}

static bool vector_test_resize_ptrs() {
	Vector<int> v;
	TEST_EQ(v.size(), 0);
	TEST_EQ(v.ptr(), nullptr);
	v.resize(5);
	TEST_EQ(v.size(), 5);
	TEST_NEQ(v.ptr(), nullptr);
	int *p = v.ptrw();
	for (int i = 0; i < 5; i++) {
		p[i] = i + 5;
	}

	int a = 5;
	for (const int &i : v) {
		TEST_EQ(i, a);
		a++;
	}

	return true;
}

static bool vector_test_cow() {
	Vector<int> v{1, 2, 3, 4, 5};
	TEST_EQ(v.get_reference_count(), 1);
	Vector<int> v2(v);
	TEST_EQ(v.size(), v2.size());
	TEST_EQ(v.ptr(), v2.ptr());
	TEST_EQ(v.get_reference_count(), 2);
	v.append(6);
	TEST_NEQ(v.size(), v2.size());
	TEST_NEQ(v.ptr(), v2.ptr());
	TEST_EQ(v.get_reference_count(), 1);
	{
		Vector<int> v3(v);
		TEST_EQ(v.get_reference_count(), 2);
	}
	TEST_EQ(v.get_reference_count(), 1);
	return true;
}

void vector_register_tests() {
	register_test(vector_test_init_empty, "Vector construction with an empty set of CowData");
	register_test(vector_test_init_list, "Vector construction from an std::initializer_list");
	register_test(vector_test_init_copy_from, "Vector construction by copying from an existing vector");
	register_test(vector_test_init_move, "Vector construction from the std::move operator");
	register_test(vector_test_assignment, "Vector assignment from other Vectors and standard classes");
	register_test(vector_test_iterator, "Vector const and non-const iterating classes");
	register_test(vector_test_get, "Vector reading data using get() and the [] operator");
	register_test(vector_test_set, "Vector writing data using set() and the [] operator");
	register_test(vector_test_find, "Vector finding data");
	register_test(vector_test_has, "Vector checking for the existence of data");
	register_test(vector_test_push, "Vector pushing data");
	register_test(vector_test_insert, "Vector inserting data");
	register_test(vector_test_erase, "Vector erasing data");
	register_test(vector_test_clear, "Vector clearing data");
	register_test(vector_test_resize_ptrs, "Vector resizing and direct pointer access");
	register_test(vector_test_cow, "Vector confirmation of proper copy-on-write semantics");
}
