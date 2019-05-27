#include <fcntl.h>
#include <unistd.h>

#include "SegmentType.h"
#include "ArgumentType.h"
#include "Constants.h"
#include "Parameter.h"
#include "CommandHandler.h"

void printbincharpad(char c) {
    for (int i = 7; i >= 5; --i) {
        putchar((c & (1 << i)) ? '1' : '0');
    }
    putchar('\n');
}

void printArgumentsHelp() {
    int fd = open("./Help.txt", O_RDONLY);

    char c;
    while (read(fd, &c, 1)) {
        printf("%c", c);
    }

    close(fd);
}

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


void handleArguments(int argc, char *argv[]) {

    if (strcmp(argv[0], "-h") == 0) {
        printArgumentsHelp();
    } else {

        if (argc < 2) {
            errx(WRONG_ARGUMENTS_CODE, "Invalid number of parameters");
        }

//        FILE *file = openFileHandleError(argv[FILE_INDEX], "rb");

        enum ArgumentType argumentType = getArgumentType(argv[ARGUMENT_INDEX]);

        executeCommand(argc, argv, argumentType);
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

    char *argv[] = {"/home/kristiyan/Downloads/60000/examples/test.bin", "-s", "device_name", "Gosho pederasa"};
    notMain(4, argv);
}