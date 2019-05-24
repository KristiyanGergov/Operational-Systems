#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

const static int ONE = 1;

enum SegmentType {
    Text,
    Digital,
    Byte
};

void printbincharpad(char c) {
    for (int i = 7; i >= 0; --i) {
        putchar((c & (1 << i)) ? '1' : '0');
    }
    putchar('\n');
}


enum SegmentType getType(char byte) {

    switch (byte) {
        case 0:
            return Text;
        case 1:
            return Digital;
        case 2:
            return Byte;
        default:
            err(1, "Invalid type: %u. Possible types: 0, 1, 2", byte);
    }

}

FILE *openFile(char *path, char *modes) {

    FILE *file = fopen(path, modes);

    if (file != NULL) {
        return file;
    } else {
        err(2, "Could't open file in read mode! File: %s", path);
    }

}

void readByteFromFile(char* buffer, FILE *file) {
    size_t items_read = fread(buffer, sizeof(buffer), 1, file);

    if (items_read == 0) {
        err(2, "Error while reading from file!");
    }
}

int main(int argc, char *argv[]) {

    char *path = "/home/kristiyan/Downloads/60000/examples/byte.bin";

    FILE *file = openFile(path, "rb"); //r for read, b for binary

    char metaType;
    char metaParams[7];

    readByteFromFile(&metaType, file);

    enum SegmentType segmentType = getType(metaType);

    readByteFromFile(metaParams, file);


//    while (read(fd1, &buff, 1)) {
////        write(1, &buff, 1);
//        printbincharpad(buff);
////        count++;
////        printf("%u\n", buff);
//    }
    return 0;
}