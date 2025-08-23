#include "error_types.h"

const char *error_messages[] = {
    "OK",
    "Failed",
    "Unavailable",
    "Invalid parameter",
    "Out of memory",
    "Item was null",
    "Item cannot be found",
    "Item cannot be created",
    "File not found",
    "File can't be accessed",
    "Help was called for",
};

static_assert(sizeof(error_messages) / sizeof(error_messages[0]) == ERR_MAX, "Expected the number of error messages to be the same as ERR_MAX");