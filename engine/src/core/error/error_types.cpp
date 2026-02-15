#include <core/error/error_types.h>

const char *error_messages[] = {
    "OK",
    "Failed",
    "Unavailable",
    "Invalid parameter",
    "Out of memory",
    "Item was null",
    "Item cannot be found",
    "Item cannot be created",
    "Item cannot be deleted",
    "File not found",
    "File can't be accessed",
    "Item already exists",
    "Item does not exist",
    "Something failed",
    "A bug in the code was found",
    "Help was called for",
};

static_assert(sizeof(error_messages) / sizeof(error_messages[0]) == ERR_MAX, "Expected the number of error messages to be the same as ERR_MAX");