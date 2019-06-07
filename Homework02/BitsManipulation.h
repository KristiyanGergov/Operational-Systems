#include "Position.h"
#include <bits/types/FILE.h>
#include <stdio.h>
#include "FileManager.h"

#define META_LENGTH 8

void changeBitParameter(char *data, struct Position position, int toBit) {

    char cPosition = (char) (META_LENGTH - position.metaBitPosition - 1);

    char mask = (char) 1 << cPosition;

    char c = data[position.metaByteParameterPosition];

    data[position.metaByteParameterPosition] = (char) ((c & ~mask) | ((toBit << cPosition) & mask));
}

int checkIfBitOptionIsActive(FILE *file, struct Position position) {

    fseek(file, position.metaByteParameterPosition, SEEK_SET);

    char c;

    readBytesFromFile(&c, 1, file);
    fseek(file, 0, SEEK_SET);
    return c & (1 << (META_LENGTH - position.metaBitPosition - 1));
}
