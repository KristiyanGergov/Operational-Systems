#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include "FileManager.h"
#include "Position.h"

#define BITS_IN_BYTE 8
#define ONE 1
#define META_TYPE_LENGTH 1
#define META_LENGTH 8
#define SEGMENT_LENGTH 64
#define PARAM_INDEX 2
#define PARAM_VALUE_INDEX 3
#define TEMP_PATH "/home/kristiyan/Fmi/OS/Homework02/Resourses/temp.txt"

int charToInt(char c) {
    return c - '0';
}

struct Parameter getParameter(char *param) {

    char *path;
    path = malloc(strlen(CONFIG_PATH) + ONE + strlen(param));
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
    parameter.segment = charToInt(line[0]);

    getline(&line, &len, file);
    parameter.position = charToInt(line[0]);

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

void print(int a) {
    for (int i = 0; i < BITS_IN_BYTE; i++) {
        printf("%d", ((a << i) & 0x80) != 0);
    }
    printf("\n");
}

void changeBitParameter(char *data, struct Position position, int toBit) {

    char cPosition = (char) (META_LENGTH - position.metaBitPosition - 1);

    char mask = (char) 1 << cPosition;

    char c = data[position.metaByteParameterPosition];
    print(data[position.metaByteParameterPosition]);

    data[position.metaByteParameterPosition] = (char) ((c & ~mask) | ((toBit << cPosition) & mask));
    print(data[position.metaByteParameterPosition]);
}

void finishTempFile(FILE *file, FILE *temp, char *fileName) {
    char c;

    while (fread(&c, ONE, ONE, file)) {
        writeBytesToFile(&c, ONE, temp);
    }

    fclose(file);
    fclose(temp);
    remove(fileName);
    rename(TEMP_PATH, fileName);
}

void replace(char *fileName, char *newValue, struct Position position, bool isCapital) {

    enum SegmentType type = position.segmentType;

    if (strlen(newValue) > type) {
        err(WRONG_ARGUMENTS_CODE, "Maximum length of value is %zd. Current %d", strlen(newValue), type);
    }

    FILE *file = openFileHandleError(fileName, READ_BINARY);
    FILE *temp = openFileHandleError(TEMP_PATH, WRITE_BINARY);

    char firstHalf[position.dataBytePosition];
    readBytesFromFile(firstHalf, (size_t) position.dataBytePosition, file);

    if (!isCapital) {
        changeBitParameter(firstHalf, position, ONE);
    }

    writeBytesToFile(firstHalf, sizeof(firstHalf), temp);
    writeBytesToFile(newValue, strlen(newValue), temp);

    for (size_t i = 0; i < type - strlen(newValue); i++)
        writeBytesToFile("\0", ONE, temp);

    fseek(file, type + position.dataBytePosition, SEEK_SET);

    finishTempFile(file, temp, fileName);
}

enum SegmentType getSegmentType(char *path, struct Parameter parameter) {

    FILE *file = openFileHandleError(path, READ_BINARY);
    fseek(file, parameter.segment * SEGMENT_LENGTH + parameter.segment, SEEK_SET);
    char typeChar;
    readBytesFromFile(&typeChar, ONE, file);
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

void assertEqualsNumberOfArguments(int actual, int expected) {
    if (expected != actual) {
        err(WRONG_ARGUMENTS_CODE, "Wrong number of arguments! Current: %d, Desired: %d", actual, expected);
    }
}

struct Position executePosition(char *path, char *param) {

    struct Parameter parameter = getParameter(param);

    enum SegmentType type = getSegmentType(path, parameter);

    return getPositionOfParameter(parameter, type);
}

void executeCommandS(char *argv[], bool isCapital) {

    struct Position positionToReplace = executePosition(argv[FILE_INDEX], argv[PARAM_INDEX]);

    char *paramValue = argv[PARAM_VALUE_INDEX];

    if (strlen(paramValue) > positionToReplace.segmentType) {
        err(WRONG_ARGUMENTS_CODE, "The maximum length of the parameter is: %d. Current: %zd",
            positionToReplace.segmentType, sizeof(paramValue));
    }

    replace(argv[FILE_INDEX], paramValue, positionToReplace, isCapital);
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
    printf("\n");
    fseek(file, 0, SEEK_SET);
}

void executeCommandG(char *argv[], bool isCapital) {

    struct Position position = executePosition(argv[FILE_INDEX], argv[PARAM_INDEX]);

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

void executeCommandL(char *argv[], int argc, bool isCapital) {

    for (int i = 2; i < argc; ++i) {
        struct Position position = executePosition(argv[FILE_INDEX], argv[i]);
        FILE *file = openFileHandleError(argv[FILE_INDEX], READ_BINARY);

        if (checkIfOptionIsActive(file, position) || isCapital) {
            printParameter(file, position);
        }
    }
}

void executeCommandB(char *argv[]) {
    char *paramValue = argv[PARAM_VALUE_INDEX];

    if (paramValue[0] != '0' && paramValue[0] != '1') {
        err(WRONG_ARGUMENTS_CODE, "Parameter must be either 0 or 1!");
    }

    struct Position position = executePosition(argv[FILE_INDEX], argv[PARAM_INDEX]);

    FILE *file = openFileHandleError(argv[FILE_INDEX], READ_BINARY);
    FILE *temp = openFileHandleError(TEMP_PATH, WRITE_BINARY);

    char firstHalf[position.dataBytePosition];
    readBytesFromFile(firstHalf, (size_t) position.dataBytePosition, file);

    changeBitParameter(firstHalf, position, charToInt(paramValue[0]));

    writeBytesToFile(firstHalf, sizeof(firstHalf), temp);

    fseek(file, position.dataBytePosition, SEEK_SET);

    finishTempFile(file, temp, argv[FILE_INDEX]);
}

void executeCommandC(int argc, char *argv[]) {

}

void executeCommand(int argc, char *argv[], enum ArgumentType argumentType) {

    switch (argumentType) {
        case s:
            assertEqualsNumberOfArguments(argc, 4);
            executeCommandS(argv, false);
            break;
        case S:
            assertEqualsNumberOfArguments(argc, 4);
            executeCommandS(argv, true);
            break;
        case g:
            assertEqualsNumberOfArguments(argc, 3);
            executeCommandG(argv, false);
            break;
        case G:
            assertEqualsNumberOfArguments(argc, 3);
            executeCommandG(argv, true);
            break;
        case l:
            executeCommandL(argv, argc, false);
            break;
        case L:
            executeCommandL(argv, argc, true);
            break;
        case b:
            executeCommandB(argv);
            break;
        case c:
        default:
            break;
    }
}