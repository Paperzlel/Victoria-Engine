#pragma once

#include "defines.h"

/* 
 * SETTING UP A FILESYSTEM
 * Step 1: create the file path
 * Step 2: Get the size of a file
 * Step 3: Allocate the appropriate space to the file
 * Step 4: Read the file
 */

typedef enum FileAccessType {
    ACCESS_TYPE_READ,
    ACCESS_TYPE_WRITE,
} FileAccessType;

typedef struct FileHandler {
    void* handle;
    b8 isValidFile;
} FileHandler;

typedef struct FileSystemConfig {
    string cwd;
    u64 memoryUsage;
} FileSystemConfig;

/* 
 * @brief Method to open a file from a given filepath of a specific access type
 * @param *fh The file handler pointer for the file
 * @param filepath The path to the given file
 * @param type The access type for the file, either `ACCESS_TYPE_READ` for reading files or `ACCESS_TYPE_WRITE` for writing to files
 * @return `TRUE` if the loader was able to load the file, `FALSE` if not
 */
VAPI b8 OpenFile(FileHandler* fh, string filepath, FileAccessType type);

VAPI void CloseFile(FileHandler* fh);

/* 
 * @brief Method to find whether a file exists at a given path or not
 * @param filepath The path to the file
 * @return `TRUE` if the file does exist, `FALSE` if it does not
 */
VAPI b8 FileExists(string filepath);

/*
 * @brief Method to get the contents of a file in text form. Useful for most 
 * @param fh The file handler 
 * @param filepath The path to the current file
 * @param *contentReturn The place to return the contents of the file to
 * @return A single string of the entire file contents
 */
VAPI b8 GetFileContentsAsText(FileHandler* fh, string filepath, char* contentReturn);

/*
 * @brief Method to get the size of a file in bytes
 * @param fh The file handler
 * @return The size of the file in bytes
 */
VAPI u64 GetFileSize(FileHandler* fh);
