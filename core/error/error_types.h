#ifndef ERROR_TYPES_H
#define ERROR_TYPES_H

enum Error {
    OK,
    FAILED,
    ERR_UNAVAILABLE,
    ERR_INVALID_PARAMETER,
    ERR_OUT_OF_MEMORY,
    ERR_NULL,
    ERR_FILE_NOT_FOUND,
    ERR_FILE_CANT_ACCESS,
    ERR_MAX
};

/* @brief String for each error message, in a more human-readable format */
extern const char *error_messages[];

#endif