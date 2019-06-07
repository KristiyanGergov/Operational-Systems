#pragma once

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
        fclose(file);
        err(ERROR_FILE_CODE, "Error while reading from file!");
    }
}

void readBytesFromPath(char *arr, size_t bytesCount, char *path) {
    FILE *file = openFileHandleError(path, READ_BINARY);
    readBytesFromFile(arr, bytesCount, file);
    fclose(file);
}

void writeBytesToFile(char *buffer, size_t bytesCount, FILE *file) {
    fwrite(buffer, bytesCount, 1, file);
}



void replaceBytesFromFileToNew(FILE *file, FILE *newFile, char *fileName) {
    char c;

    while (fread(&c, 1, 1, file)) {
        writeBytesToFile(&c, 1, newFile);
    }

    fclose(file);
    fclose(newFile);
    remove(fileName);
    rename(TEMP_PATH, fileName);
}

