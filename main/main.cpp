#include "core/io/logger.h"
#include "core/error/error_macros.h"
#include "core/variant/variant.h"
#include "core/variant/array.h"
#include "core/data/vector.h"

int main(void) {
    PRINT("Hello World!");
    PRINT_DEBUG("The number is %i", 12);
    PRINT_ERR("oops");

    Variant x = 3;
    PRINT("The number x is: %i", (int32)x);

    Array y;
    y.append(1);
    y.append(2.2);
    y.append(true);

    x = y;
    for (const int i : y) {
        PRINT("%i", (int32)y[i]);
    }

    Vector<int> vec = {1, 2, 3, 4, 5};
    for (const int item : vec) {
        PRINT("item is: %i", item);
    }

    return 0;
}