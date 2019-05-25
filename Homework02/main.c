#include <stdio.h>
#include <err.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void printbincharpad(char c) {
    for (int i = 7; i >= 5; --i) {
        putchar((c & (1 << i)) ? '1' : '0');
    }
    putchar('\n');
}

const int FILE_INDEX = 0;

const int ARGUMENT_INDEX = 1;

void printArgumentsHelp() {
    int fd = open("./Help.txt", O_RDONLY);

    char c;
    while (read(fd, &c, 1)) {
        printf("%c", c);
    }

    close(fd);
}

enum SegmentType {
    Text,
    Digital,
    Byte
};

enum ArgumentType {
    s,
    S,
    g,
    G,
    l,
    L,
    b,
    c,
    h
};

enum SegmentType getSegmentType(char byte) {

    switch (byte) {
        case 0:
            return Text;
        case 1:
            return Digital;
        case 2:
            return Byte;
        default:
            err(2, "Invalid type: %u. Possible types: 0, 1, 2", byte);
    }

}

enum ArgumentType getArgumentType(char *byte) {
    if (strlen(byte) != 2) {
        err(1, "Invalid argument: %s", byte);
    }

    switch (byte[1]) {
        case 's':
            return s;
        case 'S':
            return S;
        case 'g':
            return g;
        case 'G':
            return G;
        case 'l':
            return l;
        case 'L':
            return L;
        case 'b':
            return b;
        case 'c':
            return c;
        case 'h':
            return h;
        default:
            err(1, "Invalid argument: %u. Possible arguments: s, S, g, G, l, L, b, c, h", byte[0]);
    }
}

FILE *openFile(char *path, char *modes) {

    FILE *file = fopen(path, modes);

    if (file == NULL) {
        err(3, "Could't open file in read mode! File: %s", path);
    }

    return file;
}

void readBytesFromFile(char *buffer, size_t bytesCount, FILE *file) {
    size_t items_read = fread(buffer, bytesCount, 1, file);

    if (items_read == 0) {
        err(2, "Error while reading from file!");
    }
}

void executeCommand(int argc, char *argv[], FILE *file, enum ArgumentType argumentType) {

    switch (argumentType) {
        case s:
        case S:
        case g:
        case G:
        case l:
        case L:
        case b:
        case c:
        case h:
        default:
            break;
    }

}

void handleArguments(int argc, char *argv[]) {

    if (strcmp(argv[0], "-h") == 0) {
        printArgumentsHelp();
    } else {

        if (argc < 2) {
            errx(1, "Invalid number of parameters");
        }

        FILE *file = openFile(argv[FILE_INDEX], "rb");

        enum ArgumentType argumentType = getArgumentType(argv[ARGUMENT_INDEX]);

        executeCommand(argc, argv, file, argumentType);
    }

}

int notMain(int argc, char *argv[]) {

    handleArguments(argc, argv);
    char *path = argv[FILE_INDEX];

    FILE *file = openFile(path, "rb"); //r for read, b for binary

    char metaType;
    char metaParams[7];

    readBytesFromFile(&metaType, sizeof(metaType), file);

    enum SegmentType segmentType = getSegmentType(metaType);

    readBytesFromFile(metaParams, sizeof(metaParams), file);


    printbincharpad(metaParams[0]);


//    while (read(fd1, &buff, 1)) {
////        write(1, &buff, 1);
//        printbincharpad(buff);
////        count++;
////        printf("%u\n", buff);
//    }
    return 0;
}

int main() {
    char *argv[] = {"/home/kristiyan/Downloads/60000/examples/text.bin", "-s", "device_name"};
    notMain(3, argv);
}