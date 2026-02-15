#include "test_event.h"

#ifdef TESTS_ENABLED

#include <core/data/event.h>
#include <core/variant/variant.h>

#include <tests/test_macros.h>
#include <tests/test_manager.h>

void testfunc1() {}
void testfunc2() {}

void testfunc3(int p_arg1) {}
void testfunc4(int p_arg1, double p_arg2) {}

template<typename T>
void testfunc5(T item) {}

static bool run_event_tests() {
    CallableMethod cm1 = static_callable_mp(testfunc1);
    CallableMethod cm2 = static_callable_mp(testfunc2);
    CallableMethod cm3 = static_callable_mp(testfunc3);
    CallableMethod cm4 = static_callable_mp(testfunc4);
    CallableMethod cm5 = static_callable_mp(testfunc5<int>);
    Event<> ev1;
    // Basic creation of an Event. Adds an event, fires it, then destroys it.
    ev1.connect(cm1);
    TEST_EQ(ev1.has(cm1), true);
    ev1.fire();
    Error err = ev1.disconnect(cm1);
    TEST_EQ(err, OK);
    ev1.~Event();

    // Adding two listeners to an event, and firing them both.
    Event<> ev2;
    ev2.connect(cm1);
    TEST_EQ(ev2.has(cm1), true);
    ev2.connect(cm2);
    TEST_EQ(ev2.has(cm2), true);
    ev2.fire();
    err = ev2.disconnect(cm1);
    TEST_EQ(err, OK);
    err = ev2.disconnect(cm2);
    TEST_EQ(err, OK);
    ev2.~Event();

    // Adding listeners with one parameter
    Event<int> ev3;
    ev3.connect(cm3);
    TEST_EQ(ev3.has(cm3), true);
    ev3.fire(0);
    err = ev3.disconnect(cm3);
    TEST_EQ(err, OK);
    ev3.~Event();

    // Adding listeners with two parameters
    Event<int, double> ev4;
    ev4.connect(cm4);
    TEST_EQ(ev4.has(cm4), true);
    ev4.fire(0, 0.0);
    err = ev4.disconnect(cm4);
    TEST_EQ(err, OK);
    ev4.~Event();

    // Adding listeners with templates
    Event<int> ev5;
    ev5.connect(cm5);
    TEST_EQ(ev5.has(cm5), true);
    ev5.fire(0);
    err = ev5.disconnect(cm5);
    TEST_EQ(err, OK);
    ev5.~Event();

    return true;
}

void event_register_tests() {
    register_test(run_event_tests, "Runs all of the event tests.");
}

#endif // TESTS_ENABLED