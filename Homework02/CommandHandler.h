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

struct Position getPositionOfParameter(struct Parameter parameter, enum SegmentType type) {

    struct Position position;

    int segmentByte = SEGMENT_LENGTH * parameter.segment + parameter.segment;

    position.dataBytePosition = segmentByte + META_LENGTH + (parameter.position * type);
    position.metaBitPosition = parameter.position % META_LENGTH;
    position.metaByteParameterPosition = segmentByte + META_TYPE_LENGTH + (parameter.position % META_LENGTH);
    position.segmentType = type;
    position.metaByteTypePosition = segmentByte;

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

    FILE *file = openFileHandleError(fileName, READ_BINARY);
    FILE *temp = openFileHandleError(tempPath, WRITE_BINARY);

    char firstHalf[position.dataBytePosition];
    readBytesFromFile(firstHalf, (size_t) position.dataBytePosition, file);

    if (!isCapital) {
        firstHalf[position.metaByteParameterPosition] |= 1 << (META_LENGTH - position.metaBitPosition - 1);
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

    FILE *file = openFileHandleError(argv[FILE_INDEX], READ_BINARY);
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

struct Position executePosition(int argc, char *argv[], int desiredArgc) {

    if (argc != desiredArgc) {
        err(WRONG_ARGUMENTS_CODE, "Wrong number of arguments! Current: %d, Desired: %d", argc, desiredArgc);
    }

    char *param = argv[PARAM_INDEX];

    struct Parameter parameter = getParameter(param);

    enum SegmentType type = getSegmentType(argv, parameter);

    return getPositionOfParameter(parameter, type);
}

void executeCommandS(int argc, char *argv[], bool isCapital) {

    struct Position positionToReplace = executePosition(argc, argv, 4);

    char *paramValue = argv[PARAM_VALUE_INDEX];

    if (strlen(paramValue) > positionToReplace.segmentType) {
        err(WRONG_ARGUMENTS_CODE, "The maximum length of the parameter is: %d. Current: %zd",
            positionToReplace.segmentType, sizeof(paramValue));
    }

    replace(argv[FILE_INDEX], paramValue, positionToReplace, isCapital, positionToReplace.segmentType);
}

int checkIfOptionIsActive(FILE *file, struct Position position) {

    fseek(file, position.metaByteParameterPosition, SEEK_SET);

    char c;

    readBytesFromFile(&c, 1, file);
    fseek(file, 0, SEEK_SET);
    return c & (1 << (META_LENGTH - position.metaBitPosition - 1));
}

void printParameter(FILE *file, struct Position position) {
    fseek(file, position.dataBytePosition, SEEK_SET);

    char c;

    int maxCharsToRead = position.segmentType;
    int charsRead = 0;

    readBytesFromFile(&c, 1, file);

    while (c && charsRead < maxCharsToRead) {
        charsRead++;
        printf("%c", c);
        readBytesFromFile(&c, 1, file);
    }
}

void executeCommandG(int argc, char *argv[], bool isCapital) {

    struct Position position = executePosition(argc, argv, 3);

    FILE *file = openFileHandleError(argv[FILE_INDEX], READ_BINARY);

    if (!isCapital) {
        if (!checkIfOptionIsActive(file, position)) {
            fclose(file);
            err(ERROR_FILE_CODE, "Option not activated!");
        }
    }

    printParameter(file, position);
    fclose(file);
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
            break;
        case G:
            executeCommandG(argc, argv, true);
            break;
        case l:
        case L:
        case b:
        case c:
        case h:
        default:
            break;
    }

}