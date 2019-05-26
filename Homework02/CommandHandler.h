#ifndef HOMEWORK02_COMMANDHANDLER_H
#define HOMEWORK02_COMMANDHANDLER_H

#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include "FileManager.h"

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
#endif
