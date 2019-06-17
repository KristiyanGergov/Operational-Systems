#include <bits/types/FILE.h>
#include <stddef.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "comm.h"

#define FILE_LENGTH 32

int open_file(char *path, int flags) {
    int fd;
    if ((fd = open(path, flags)) == -1) {
        error(strcat("Error while opening file: ", path));
    }

    return fd;
}

int fileExist(char *path) {
    return access(path, F_OK) != -1;
}

void write_bytes_to_fd(int fd, uint32_t *file, size_t bytesToWrite) {
    size_t bytesWritten = write(fd, file, bytesToWrite);

    if (bytesWritten != bytesToWrite)
        error("Error while writing bytes to file.");
}

void read_bytes_from_fd(int fd, uint32_t *bank) {

    ssize_t bytesRead = read(fd, bank, FILE_LENGTH);
    if (bytesRead != FILE_LENGTH)
        error("Error while reading bytes from file.");
}

int read_or_default_file(char *path, uint32_t *bank) {

    int fd;

    if (!fileExist(path)) {
        fd = open_file(path, O_CREAT);
        uint32_t temp;
        write_bytes_to_fd(fd, &temp, FILE_LENGTH);

    } else {
        fd = open_file(path, O_RDWR);
        read_bytes_from_fd(fd, bank);
    }

    return fd;
}