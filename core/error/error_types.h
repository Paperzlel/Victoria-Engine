#pragma once
enum Error {
    OK,
    FAILED,
    ERR_UNAVAILABLE,
    ERR_INVALID_PARAMETER,
    ERR_OUT_OF_MEMORY,
    ERR_NULL,
    ERR_CANT_FIND,
    ERR_CANT_CREATE,
    ERR_FILE_NOT_FOUND,
    ERR_FILE_CANT_ACCESS,
    ERR_HELP,
    ERR_MAX
};

/* @brief String for each error message, in a more human-readable format */
extern const char *error_messages[];
