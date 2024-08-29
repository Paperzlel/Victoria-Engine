#include "core/io/logger.h"
#include "core/error/error_macros.h"

int main(void) {
    PRINT("Hello World!");
    PRINT_DEBUG("The number is %i", 12);
    PRINT_ERR("oops");
    ERR_FAIL_COND_MSG_R(true, "testing :P", 0);
    return 0;
}