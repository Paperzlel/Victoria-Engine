#include "core/io/logger.h"
#include "core/error/error_macros.h"
#include "core/variant/variant.h"

int main(void) {
    PRINT("Hello World!");
    PRINT_DEBUG("The number is %i", 12);
    PRINT_ERR("oops");

    Variant x = 3;
    PRINT("The number x is: %i", (int32)x);

    return 0;
}