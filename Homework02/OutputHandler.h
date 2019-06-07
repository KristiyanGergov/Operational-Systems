#include <stdio.h>
#include "Constants.h"
#include "Position.h"
#include "FileManager.h"

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