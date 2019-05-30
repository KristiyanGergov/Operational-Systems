#include <bits/types/FILE.h>
#include <stdio.h>
#include <err.h>
#include "Constants.h"

FILE *openFile(char *path, char *modes) {
    return fopen(path, modes);
}

FILE *openFileHandleError(char *path, char *modes) {
    FILE *file = openFile(path, modes);

    if (file == NULL) {
        err(ERROR_FILE_CODE, "Could't open file in read mode! File: %s", path);
    }

    return file;
}

void readBytesFromFile(char *buffer, size_t bytesCount, FILE *file) {
    size_t items_read = fread(buffer, bytesCount, 1, file);

    if (items_read == 0) {
        err(ERROR_FILE_CODE, "Error while reading from file!");
    }
}

void writeBytesToFile(char* buffer, size_t bytesCount, FILE* file) {

    fwrite(buffer, bytesCount, 1, file);
}