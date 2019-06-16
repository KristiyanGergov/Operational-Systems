#include <bits/types/FILE.h>
#include <stddef.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "comm.h"

FILE *open_file(char *path, char *modes) {
    FILE *file = fopen(path, modes);

    if (file == NULL) {
        error(strcat("Could't open file in read mode! File: %s\n", path));
    }

    return file;
}

size_t readBytesFromFile(uint32_t *buffer, size_t bytesCount, size_t memb, FILE *file) {
    size_t items_read = fread(buffer, bytesCount, memb, file);

    if (items_read == 0 || items_read != memb) {
        fclose(file);
        err(1, "Error while reading from file!\n Files read %ld   %ld", items_read, bytesCount);
    }
    return items_read;
}

size_t writeBytesToFile(uint32_t *buffer, size_t bytesCount, size_t memb, FILE *file) {
    size_t bytesWritten = fwrite(buffer, bytesCount, memb, file);

    if (bytesWritten != bytesCount * memb) {
        fclose(file);
        error("Error while writing to file!\n");
    }

    return bytesWritten;
}

int fileExist(char *filename) {
    return access(filename, F_OK) != -1;
}