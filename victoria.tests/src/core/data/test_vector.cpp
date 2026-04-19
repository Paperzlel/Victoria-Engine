#include "core/data/test_vector.h"

#include "test_macros.h"
#include "test_manager.h"

#include <core/data/vector.h>

struct vectortest1 {
	int a;
	bool b;
};

class ATestClass {
	int a = 0;
	vectortest1 other = {};

public:
	ATestClass() {
		a = 12;
		other = {1, true};
	}

	ATestClass(const ATestClass &p_other) {
		a = p_other.a;
		other = p_other.other;
	}

	~ATestClass() {
		a = 0;
		other = {};
	}
};

static bool vector_test_basic() {
	Vector<int> vec;
	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);
	TEST_EQ(vec[0], 1);
	TEST_EQ(vec[1], 2);
	TEST_EQ(vec[2], 3);

	vec.clear();
	TEST_EQ(vec.size(), 0);

	return true;
}

static bool vector_test_lifetimes() {
	Vector<int> vec = {1, 2, 3, 4, 5, 6};
	TEST_EQ(vec.get_reference_count(), 1);
	Vector<int> vec2 = vec;
	// Different scope
	{
		Vector<int> vec3 = vec2;
		TEST_EQ(vec3.get_reference_count(), 3);
	}
	TEST_EQ(vec2.get_reference_count(), 2);
	vec2.clear();
	TEST_EQ(vec.get_reference_count(), 1);
	return true;
}

static bool vector_test_struct() {
	Vector<vectortest1> vec;
	vectortest1 v;
	v.a = 1;
	v.b = false;
	vec.push_back(v);
	TEST_EQ(vec[0].a, v.a);
	TEST_EQ(vec[0].b, v.b);

	vec.clear();
	TEST_EQ(vec.size(), 0);

	return true;
}

static bool vector_test_nontrivial_class() {
	Vector<ATestClass> vec;
	ATestClass a;
	vec.append(a);
	vec.append(a);
	TEST_EQ(vec.size(), 2);
	vec.insert(a, 1);
	vec.remove_at(1);
	TEST_EQ(vec.size(), 2);
	TEST_EQ(vec.get_reference_count(), 1);
	Vector<ATestClass> other = vec;
	TEST_EQ(vec.get_reference_count(), 2);
	vec.append(a);
	TEST_EQ(vec.get_reference_count(), 1);
	other.clear();
	vec.clear();
	TEST_EQ(vec.get_reference_count(), 0);
	return true;
}

static bool vector_test_search() {
	Vector<int> vec;
	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(4);
	vec.push_back(8);
	vec.push_back(16);

	TEST_EQ(vec.find(1), 0);
	TEST_EQ(vec.find(2), 1);
	TEST_EQ(vec.find(4), 2);
	TEST_EQ(vec.find(8), 3);
	TEST_EQ(vec.find(16), 4);
	TEST_EQ(vec.find(32), -1);

	return true;
}

static bool vector_test_push_pop() {
	Vector<int> vec;
	vec.push_back(2);
	vec.push_back(4);
	vec.push_back(6);
	TEST_EQ(vec[0], 2);
	TEST_EQ(vec[1], 4);
	TEST_EQ(vec[2], 6);
	TEST_EQ(vec.size(), 3);

	vec.insert(1, 0);
	TEST_EQ(vec[0], 1);
	TEST_EQ(vec.size(), 4);

	vec.insert(3, 2);
	TEST_EQ(vec[2], 3);
	TEST_EQ(vec.size(), 5);

	int back = vec.get(vec.size() - 1);
	vec.remove_at(vec.size() - 1);
	TEST_EQ(back, 6);
	TEST_EQ(vec.size(), 4);

	int front = vec.get(0);
	vec.remove_at(0);
	TEST_EQ(front, 1);
	TEST_EQ(vec.size(), 3);
	vec.insert(1, 0);
	TEST_EQ(vec[0], 1);
	TEST_EQ(vec.size(), 4);

	vec.remove_at(3);
	TEST_EQ(vec.size(), 3);

	return true;
}

static bool vector_test_pointers() {
	Vector<int> vec;
	vec.push_back(1);
	vec.push_back(1024);
	vec.push_back(32768);
	TEST_EQ(vec[0], 1);
	TEST_EQ(vec[1], 1024);
	TEST_EQ(vec[2], 32768);

	int *ptr = vec.ptrw();
	ptr[1] = 32;
	TEST_EQ(vec[1], ptr[1]);

	const int *ptr2 = vec.ptr();
	vec[0] = 4;
	TEST_EQ(ptr2, vec.ptr());

	return true;
}

static bool vector_test_iterators_constructors() {
	Vector<int> vec = {1, 2, 3, 4, 5};
	TEST_EQ(vec[0], 1);
	TEST_EQ(vec[1], 2);
	TEST_EQ(vec[2], 3);
	TEST_EQ(vec[3], 4);
	TEST_EQ(vec[4], 5);

	int count = 1;
	for (const int &i : vec) {
		TEST_EQ(i, count);
		count++;
	}
	Vector<int> vec2 = vec;
	TEST_EQ(vec2, vec);
	TEST_EQ(vec.is_empty(), false);

	return true;
}

void vector_register_tests() {
	register_test(vector_test_basic, "Vector reading, writing and clearing with atomic datatypes");
	register_test(vector_test_lifetimes, "Vector refcounting lifetimes");
	register_test(vector_test_struct, "Vector reading, writing and clearing with a struct");
	register_test(vector_test_nontrivial_class, "Vector reading, writing and clearing with a nontrivial class");
	register_test(vector_test_search, "Vector searching for items that exist and do not exist within itself");
	register_test(vector_test_push_pop, "Vector pushing and popping");
	register_test(vector_test_pointers, "Vector reading and writing to direct pointers");
	register_test(vector_test_iterators_constructors, "Vector constructors, destructors and iterators");
}
