#include "error_types.h"

const char *error_messages[] = {
    "OK",
    "Failed",
    "Unavailable",
    "Invalid parameter",
    "Out of memory",
    "Item was null",
    "File not found",
    "File can't be accessed",
};

static_assert(sizeof(error_messages) / sizeof(error_messages[0]) == ERR_MAX, "Expected the number of error messages to be the same as ERR_MAX");