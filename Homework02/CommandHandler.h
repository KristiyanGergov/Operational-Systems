#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <dirent.h>
#include <regex.h>
#include <stdlib.h>
#include "Position.h"
#include "OutputHandler.h"
#include "BitsManipulation.h"

#define META_TYPE_LENGTH 1
#define SEGMENT_LENGTH 64
#define FILE_INDEX 1
#define ARGUMENT_INDEX 2
#define PARAM_INDEX 3
#define PARAM_VALUE_INDEX 4
#define MAX_MATCHES 1

int charToInt(char c) {
    return c - '0';
}

struct Parameter getParameter(char *param) {

    char *path;
    path = malloc(strlen(CONFIG_PATH) + 1 + strlen(param));
    strcpy(path, CONFIG_PATH);
    strcat(path, param);

    FILE *file = openFile(path, "r");

    if (file == NULL) {
        err(WRONG_ARGUMENTS_CODE, "%s is not a valid parameter\n", param);
    }

    char *line;
    size_t len = 0;

    getline(&line, &len, file);

    struct Parameter parameter;

    parameter.segment = charToInt(line[0]);

    getline(&line, &len, file);
    parameter.position = charToInt(line[0]);

    getline(&line, &len, file);
    parameter.regex = line;

    return parameter;
}

struct Position getPositionOfParameter(struct Parameter parameter, enum SegmentType type) {

    struct Position position;

    int segmentByte = SEGMENT_LENGTH * parameter.segment;

    position.dataBytePosition = segmentByte + META_LENGTH + (parameter.position * type);
    position.metaBitPosition = parameter.position % META_LENGTH;
    position.metaByteParameterPosition = segmentByte + META_TYPE_LENGTH + (parameter.position % META_LENGTH);
    position.segmentType = type;

    return position;
}


void replace(char *fileName, char *newValue, struct Position position, bool isCapital) {

    enum SegmentType type = position.segmentType;

    if (strlen(newValue) > type) {
        err(WRONG_ARGUMENTS_CODE, "Maximum length of value is %zd. Current %d\n", strlen(newValue), type);
    }

    FILE *file = openFileHandleError(fileName, READ_BINARY);
    FILE *temp = openFileHandleError(TEMP_PATH, WRITE_BINARY);

    char firstHalf[position.dataBytePosition];
    readBytesFromFile(firstHalf, (size_t) position.dataBytePosition, file);

    if (!isCapital) {
        changeBitParameter(firstHalf, position, 1);
    }

    writeBytesToFile(firstHalf, sizeof(firstHalf), temp);
    writeBytesToFile(newValue, strlen(newValue), temp);

    for (size_t i = 0; i < type - strlen(newValue); i++)
        writeBytesToFile("\0", 1, temp);

    fseek(file, type + position.dataBytePosition, SEEK_SET);

    replaceBytesFromFileToNew(file, temp, fileName);
}

enum SegmentType getSegmentType(char *path, struct Parameter parameter) {

    FILE *file = openFileHandleError(path, READ_BINARY);
    fseek(file, parameter.segment * SEGMENT_LENGTH, SEEK_SET);
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
            err(INVALID_TYPE, "Invalid type: %u. Possible types: 0, 1, 2\n", typeChar);
    }
}

void assertEqualsNumberOfArguments(int actual, int expected) {
    if (expected != actual) {
        err(WRONG_ARGUMENTS_CODE, "Wrong number of arguments! Current: %d, Desired: %d\n", actual, expected);
    }
}

struct Position executePosition(char *path, char *param) {

    struct Parameter parameter = getParameter(param);

    enum SegmentType type = getSegmentType(path, parameter);

    return getPositionOfParameter(parameter, type);
}

//Sorry uncle Bob :(
void readAndPrintParameter(char *filePath, char *param, bool isCapital) {

    struct Position position = executePosition(filePath, param);
    FILE *file = openFileHandleError(filePath, READ_BINARY);

    if (checkIfBitOptionIsActive(file, position) || isCapital) {
        printParameter(file, position);
    }
    fclose(file);
}


void matchParamValue(char *pattern, char *paramValue) {

    int rv;
    regex_t exp;

    rv = regcomp(&exp, pattern, REG_EXTENDED);
    if (rv != 0) {
        printf("regcomp failed with %d\n", rv);
    }

    regmatch_t matches[MAX_MATCHES];

    if (regexec(&exp, paramValue, MAX_MATCHES, matches, 0) != 0 ||
        matches[0].rm_so != 0 ||
        matches[0].rm_eo != strlen(paramValue)) {
        err(WRONG_ARGUMENTS_CODE, "Invalid argument: %s. Valid values must match: %s\n", paramValue, pattern);
    }

    regfree(&exp);
}

void executeCommandS(char *argv[], bool isCapital) {

    struct Position positionToReplace = executePosition(argv[FILE_INDEX], argv[PARAM_INDEX]);
    char *paramValue = argv[PARAM_VALUE_INDEX];

    struct Parameter parameter = getParameter(argv[PARAM_INDEX]);
    matchParamValue(parameter.regex, paramValue);

    if (strlen(paramValue) > positionToReplace.segmentType) {
        err(WRONG_ARGUMENTS_CODE, "The maximum length of the parameter is: %d. Current: %zd\n",
            positionToReplace.segmentType, sizeof(paramValue));
    }

    replace(argv[FILE_INDEX], paramValue, positionToReplace, isCapital);
}

void executeCommandG(char *argv[], bool isCapital) {

    struct Position position = executePosition(argv[FILE_INDEX], argv[PARAM_INDEX]);

    FILE *file = openFileHandleError(argv[FILE_INDEX], READ_BINARY);

    if (!isCapital) {
        if (!checkIfBitOptionIsActive(file, position)) {
            fclose(file);
            err(ERROR_FILE_CODE, "Option not activated!\n");
        }
    }

    printParameter(file, position);
    fclose(file);
}

void executeCommandL(char *argv[], int argc, bool isCapital) {

    if (argc % 2 == 0) {
        err(WRONG_ARGUMENTS_CODE, "Number of arguments must be an odd number! Number of arguments: %d\n", argc);
    }

    if (argc == 3) {
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(CONFIG_DIR_PATH))) {
            while ((ent = readdir(dir)) != NULL) {
                if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
                    continue;
                readAndPrintParameter(argv[FILE_INDEX], ent->d_name, isCapital);
            }
        }
        closedir(dir);
    } else {
        for (int i = 3; i < argc; ++i) {
            readAndPrintParameter(argv[FILE_INDEX], argv[i], isCapital);
        }
    }
}

void executeCommandB(char *argv[]) {
    char *paramValue = argv[PARAM_VALUE_INDEX];

    if (paramValue[0] != '0' && paramValue[0] != '1') {
        err(WRONG_ARGUMENTS_CODE, "Parameter must be either 0 or 1!\n");
    }

    struct Position position = executePosition(argv[FILE_INDEX], argv[PARAM_INDEX]);

    FILE *file = openFileHandleError(argv[FILE_INDEX], READ_BINARY);
    FILE *temp = openFileHandleError(TEMP_PATH, WRITE_BINARY);

    char firstHalf[position.dataBytePosition];
    readBytesFromFile(firstHalf, (size_t) position.dataBytePosition, file);

    changeBitParameter(firstHalf, position, charToInt(paramValue[0]));

    writeBytesToFile(firstHalf, sizeof(firstHalf), temp);

    fseek(file, position.dataBytePosition, SEEK_SET);

    replaceBytesFromFileToNew(file, temp, argv[FILE_INDEX]);
}

void executeCommandC(int argc, char *argv[]) {

    int typesLength = (argc - 3) / 2;

    char segmentsContent[typesLength][SEGMENT_LENGTH + 1];

    for (int i = 3; i < argc; i += 2) {

        int index = charToInt(argv[i][0]);
        char type = argv[i + 1][0];

        switch (type) {
            case 't':
                readBytesFromPath(segmentsContent[index], SEGMENT_LENGTH, DEF_TEXT_FILE_PATH);
                break;
            case 'n':
                readBytesFromPath(segmentsContent[index], SEGMENT_LENGTH, DEF_NUM_FILE_PATH);
                break;
            case 'b':
                readBytesFromPath(segmentsContent[index], SEGMENT_LENGTH, DEF_BYTE_FILE_PATH);
                break;

            default:
                err(WRONG_ARGUMENTS_CODE, "Type: %c is not a valid parameter!\n", type);
        }

    }

    FILE *file = openFileHandleError(argv[FILE_INDEX], WRITE_CREATE);

    for (int j = 0; j < typesLength; ++j) {
        writeBytesToFile(segmentsContent[j], SEGMENT_LENGTH, file);
    }
    fclose(file);
}

void executeCommand(int argc, char *argv[], enum ArgumentType argumentType) {

    switch (argumentType) {
        case s:
            assertEqualsNumberOfArguments(argc, 5);
            executeCommandS(argv, false);
            break;
        case S:
            assertEqualsNumberOfArguments(argc, 5);
            executeCommandS(argv, true);
            break;
        case g:
            assertEqualsNumberOfArguments(argc, 4);
            executeCommandG(argv, false);
            break;
        case G:
            assertEqualsNumberOfArguments(argc, 4);
            executeCommandG(argv, true);
            break;
        case l:
            executeCommandL(argv, argc, false);
            break;
        case L:
            executeCommandL(argv, argc, true);
            break;
        case b:
            assertEqualsNumberOfArguments(argc, 5);
            executeCommandB(argv);
            break;
        case c:
            executeCommandC(argc, argv);
            break;
        default:
            err(WRONG_ARGUMENTS_CODE, "Invalid argument! Possible arguments: s, S, g, G, l, L, b, c, h\n");
    }
}