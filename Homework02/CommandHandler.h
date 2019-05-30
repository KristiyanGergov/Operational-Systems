#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include "FileManager.h"
#include "Position.h"

#define META_TYPE_LENGTH 1
#define META_LENGTH 8
#define SEGMENT_LENGTH 64
#define PARAM_INDEX 2
#define PARAM_VALUE_INDEX 3

struct Parameter getParameter(char *param) {

    char *path;
    path = malloc(strlen(CONFIG_PATH) + 1 + strlen(param));
    strcpy(path, CONFIG_PATH);
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

struct Position getPositionToReplace(struct Parameter parameter, enum SegmentType type) {

    struct Position position;

    int segmentByte = SEGMENT_LENGTH * parameter.segment + parameter.segment;

    position.dataBytePosition = segmentByte + META_LENGTH + (parameter.position * type);
    position.metaBitPosition = parameter.position % META_LENGTH;
    position.metaBytePosition = segmentByte + META_TYPE_LENGTH + (parameter.position % META_LENGTH);

    return position;
}

void print(char a) {
    for (int i = 0; i < 8; i++) {
        printf("%d", ((a << i) & 0x80) != 0);
    }
    printf("\n");
}

void replace(char *fileName, char *newValue, struct Position position, bool isCapital, enum SegmentType type) {

    if (strlen(newValue) > type) {
        err(WRONG_ARGUMENTS_CODE, "Maximum length of value is %zd. Current %d", strlen(newValue), type);
    }
    //todo rename
    char *tempPath = "/home/kristiyan/Fmi/OS/Homework02/Resourses/temp.txt";

    FILE *file = openFileHandleError(fileName, "rb");
    FILE *temp = openFileHandleError(tempPath, "wb");

    char firstHalf[position.dataBytePosition];
    readBytesFromFile(firstHalf, (size_t) position.dataBytePosition, file);

    if (!isCapital) {
        firstHalf[position.metaBytePosition] |= 1 << (META_LENGTH - position.metaBitPosition - 1);
    }

    writeBytesToFile(firstHalf, sizeof(firstHalf), temp);
    writeBytesToFile(newValue, strlen(newValue), temp);

    for (size_t i = 0; i < type - strlen(newValue); i++)
        writeBytesToFile("\0", 1, temp);

    fseek(file, type + position.dataBytePosition, SEEK_SET);

    char c;

    while (fread(&c, 1, 1, file)) {
        writeBytesToFile(&c, 1, temp);
    }

    fclose(file);
    fclose(temp);
    remove(fileName);
    rename(tempPath, fileName);
}

enum SegmentType getSegmentType(char *argv[], struct Parameter parameter) {

    FILE *file = openFileHandleError(argv[FILE_INDEX], "rb");
    fseek(file, parameter.segment * SEGMENT_LENGTH + parameter.segment, SEEK_SET);
    char typeChar;
    readBytesFromFile(&typeChar, 1, file);
    fclose(file);

    switch (typeChar) {
        case 0:
            return Text;
        case 1:
            return Digital;
        case 2:
            return Byte;
        default:
            err(INVALID_TYPE, "Invalid type: %u. Possible types: 0, 1, 2", typeChar);
    }

}

void executeCommandS(int argc, char *argv[], bool isCapital) {

    if (argc != 4) {
        err(WRONG_ARGUMENTS_CODE, "Wrong number of arguments! Current: %d, Desired: %d", argc, 4);
    }

    char *param = argv[PARAM_INDEX];
    char *paramValue = argv[PARAM_VALUE_INDEX];

    struct Parameter parameter = getParameter(param);

    enum SegmentType type = getSegmentType(argv, parameter);

    if (strlen(paramValue) > type) {
        err(WRONG_ARGUMENTS_CODE, "The maximum length of the parameter is: %d. Current: %zd", type, sizeof(paramValue));
    }

    struct Position positionToReplace = getPositionToReplace(parameter, type);

    replace(argv[FILE_INDEX], paramValue, positionToReplace, isCapital, type);
}

void executeCommandG(int argc, char* argv[], bool isCapital) {

    if (argc != 3) {
        err(WRONG_ARGUMENTS_CODE, "Wrong number of arguments! Current: %d, Desired: %d", argc, 3);
    }

    char *param = argv[PARAM_INDEX];

    struct Parameter parameter = getParameter(param);

    enum SegmentType type = getSegmentType(argv, parameter);

    struct Position position = getPositionToReplace(parameter, type);



}

void executeCommand(int argc, char *argv[], enum ArgumentType argumentType) {


    switch (argumentType) {
        case s:
            executeCommandS(argc, argv, false);
            break;
        case S:
            executeCommandS(argc, argv, true);
            break;
        case g:
            executeCommandG(argc, argv, false);
        case G:
            executeCommandG(argc, argv, true);
        case l:
        case L:
        case b:
        case c:
        case h:
        default:
            break;
    }

}