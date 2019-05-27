#ifndef HOMEWORK02_COMMANDHANDLER_H
#define HOMEWORK02_COMMANDHANDLER_H

#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include "FileManager.h"

struct Parameter getParameter(char *param) {

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
    parameter.segment = line[0] - '0';

    getline(&line, &len, file);
    parameter.position = line[0] - '0';

    getline(&line, &len, file);
    parameter.validValues = line;

    return parameter;
}


void executeCommandS(int argc, char *argv[], bool isCapital) {

    if (argc != 4) {
        err(WRONG_ARGUMENTS_CODE, "Wrong number of arguments! Current: %d, Desired: %d", argc, 4);
    }

    char *param = argv[2];
    char *paramValue = argv[3];

    struct Parameter parameter = getParameter(param);
    readSegmentN(argv, parameter.segment);
}

void executeCommand(int argc, char *argv[], enum ArgumentType argumentType) {

    switch (argumentType) {
        case s:
            executeCommandS(argc, argv, false);
        case S:
            executeCommandS(argc, argv, true);
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
