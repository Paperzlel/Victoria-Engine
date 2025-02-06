#include "core/io/logger.h"
#include "core/error/error_macros.h"
#include "core/variant/variant.h"
#include "core/variant/array.h"
#include "core/data/vector.h"
#include "core/data/event.h"

void test_func_1(int arg) {
    PRINT_DEBUG("The number was: %i", arg);
}

void test_func_2(int arg2) {
    PRINT("This fired second and the number was %i", arg2);
}

int main(void) {
    PRINT("Hello World!");
    PRINT_ERR("oops");

    Variant x = 3;
    PRINT("The number x is: %i", (int32)x);

    Array y;
    y.append(1);
    y.append(2.2);
    y.append(true);

    for (const int i : y) {
        PRINT("%i", (int32)y[i]);
    }

    Vector<int> vec = {1, 2, 3, 4, 5};
    for (const int item : vec) {
        PRINT("item is: %i", item);
    }

    Event<int> ev;
    ev.connect(test_func_1, false);
    ev.connect(test_func_2, false);

    ev.fire(42);

    ev.disconnect(test_func_1);
    ev.disconnect(test_func_2);

    return 0;
}