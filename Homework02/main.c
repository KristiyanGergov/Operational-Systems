#include <stdio.h>
#include <err.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <malloc.h>

void printbincharpad(char c) {
    for (int i = 7; i >= 5; --i) {
        putchar((c & (1 << i)) ? '1' : '0');
    }
    putchar('\n');
}

const int FILE_INDEX = 0;

const int ARGUMENT_INDEX = 1;

const int WRONG_ARGUMENTS_CODE = 1;

const int INVALID_TYPE = 2;

const int ERROR_FILE_CODE = 3;

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
            err(INVALID_TYPE, "Invalid type: %u. Possible types: 0, 1, 2", byte);
    }

}

enum ArgumentType getArgumentType(char *byte) {
    if (strlen(byte) != 2) {
        err(WRONG_ARGUMENTS_CODE, "Invalid argument: %s", byte);
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
            err(WRONG_ARGUMENTS_CODE, "Invalid argument: %u. Possible arguments: s, S, g, G, l, L, b, c, h", byte[1]);
    }
}


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

struct Parameter {
    char *parameter;
    char segment;
    char position;
    char *validValues;
};

struct Parameter getParameter(char *param) {

    const char *config = "/home/kristiyan/Fmi/OS/Homework02/config/";

    char *path;
    path = malloc(strlen(config) + 1 + strlen(param));
    strcpy(path, config);
    strcat(path, param);

    FILE *file = openFile(path, "r");

    if (file == NULL) {
        err(WRONG_ARGUMENTS_CODE, "%s is not a valid parameter", param);
    }

    char *line;
    size_t len = 0;

    getline(&line, &len, file);

    struct Parameter parameter;

    parameter.parameter = param;
    parameter.segment = line[0];

    getline(&line, &len, file);
    parameter.position = line[0];

    getline(&line, &len, file);
    parameter.validValues = line;

    return parameter;
}

void executeCommandS(int argc, char *argv[], FILE *file, bool isCapital) {

    if (argc != 4) {
        err(WRONG_ARGUMENTS_CODE, "Wrong number of arguments! Current: %d, Desired: %d", argc, 4);
    }

    char *param = argv[2];
    char *paramValue = argv[3];

    struct Parameter parameter = getParameter(param);

    char buff[65];

    readBytesFromFile(buff, 64, file);
}

void executeCommand(int argc, char *argv[], FILE *file, enum ArgumentType argumentType) {

    switch (argumentType) {
        case s:
            executeCommandS(argc, argv, file, false);
        case S:
            executeCommandS(argc, argv, file, true);
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

        FILE *file = openFileHandleError(argv[FILE_INDEX], "rb");

        enum ArgumentType argumentType = getArgumentType(argv[ARGUMENT_INDEX]);

        executeCommand(argc, argv, file, argumentType);
    }

}

int notMain(int argc, char *argv[]) {

    handleArguments(argc, argv);
    char *path = argv[FILE_INDEX];

    FILE *file = openFileHandleError(path, "rb"); //r for read, b for binary

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

//    getParameter("audio_bitrate");

    char *argv[] = {"/home/kristiyan/Downloads/60000/examples/text.bin", "-s", "device_name", "Gosho pederasa"};
    notMain(4, argv);
}