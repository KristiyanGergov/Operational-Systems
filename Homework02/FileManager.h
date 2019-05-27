#ifndef HOMEWORK02_FILEMANAGER_H
#define HOMEWORK02_FILEMANAGER_H

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

void readSegmentN(char *argv[], int n) {

    FILE *fd = openFileHandleError(argv[FILE_INDEX], "rb");

    char str[65];
    int loop;

    for (loop = 0; loop <= n; ++loop) {
        if (0 == fgets(str, sizeof(str), fd)) {
            break;
        }
    }

}

#endif
