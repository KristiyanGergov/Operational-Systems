#include <fcntl.h>
#include <unistd.h>

#include "SegmentType.h"
#include "ArgumentType.h"
#include "Constants.h"
#include "Parameter.h"
#include "CommandHandler.h"

void printArgumentsHelp() {
    int fd = open("./Help.txt", O_RDONLY);
    //todo
    char c;
    while (read(fd, &c, 1)) {
        printf("%c", c);
    }

    close(fd);
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

        enum ArgumentType argumentType = getArgumentType(argv[ARGUMENT_INDEX]);

        executeCommand(argc, argv, argumentType);
    }

}

int notMain(int argc, char *argv[]) {

    handleArguments(argc, argv);
    return 0;
}

int main() {
//    char *argv[] = {"/home/kristiyan/Fmi/OS/Homework02/Resourses/tt.bin", "-G", "device_name"};
//    notMain(3, argv);

    char *argv[] = {"/home/kristiyan/Fmi/OS/Homework02/Resourses/tt.bin", "-s", "device_name", "Device_n1"};
    notMain(4, argv);
}