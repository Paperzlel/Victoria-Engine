#pragma once

// The core error enum. Contains every error value the engine can handle.
enum Error {
	OK,
	FAILED,
	ERR_UNAVAILABLE,
	ERR_INVALID_PARAMETER,
	ERR_OUT_OF_MEMORY,
	ERR_NULL,
	ERR_CANT_FIND,
	ERR_CANT_CREATE,
	ERR_CANT_DELETE,
	ERR_FILE_NOT_FOUND,
	ERR_FILE_CANT_ACCESS,
	ERR_ALREADY_EXISTS,
	ERR_DOESNT_EXIST,
	ERR_FAILED,
	ERR_BUG,
	ERR_HELP,
	ERR_MAX
};

// List of error message strings to convert from the enum to a human-readable format. DO NOT USE OUTSIDE OF THE CORE
// ENGINE: USE `get_error_message()`.
extern const char *error_messages[];
