#include "file-system.h"

#include "core/vmemory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

b8 OpenFile(FileHandler* fh, string filepath, FileAccessType type) {
    switch (type) {
        case ACCESS_TYPE_READ:
            fh->handle = fopen(filepath, "r");
            break;
        case ACCESS_TYPE_WRITE:
            fh->handle = fopen(filepath, "w");
            break;
        default:
            // Assume read is default option
            fh->handle = fopen(filepath, "r");
    }
    if (fh->handle == NULL) {
        VERROR("Filepath does not exist.");
        fclose((FILE*)fh->handle);
        return FALSE;
    }

    return TRUE;
}

void CloseFile(FileHandler* fh) {
    if (fh->handle) {
        fclose((FILE*)fh->handle);
        fh->handle = 0;
    }
}

u64 GetFileSize(FileHandler* fh) {
    if (fh->handle) {
        u64 fileSize = 0;
        fseek((FILE*)fh->handle, 0, SEEK_END);
        fileSize = ftell((FILE*)fh->handle);
        rewind((FILE*)fh->handle);
        return fileSize;
    }
    // No valid file handler was passed, assume accidental
    return 0;
}

b8 GetFileContentsAsText(FileHandler* fh, string filepath, char* contentReturn) {
    if (fh->handle) {
        u64 fileSize = GetFileSize(fh);

        u64 outBytes = 0;
        outBytes = fread(contentReturn, 1, fileSize, (FILE*)fh->handle);
        fclose(fh->handle);
        return TRUE;
    }
    return FALSE;
}