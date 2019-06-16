#ifndef CLIENTSERVER_COMM_H
#define CLIENTSERVER_COMM_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "err.h"

#define CREATE_MODE 0660

uint8_t isValidAccount[128];

void initValidAccount() {
    for (int j = 0; j < 128; ++j) {
        isValidAccount[j] = 0;
    }

    for (int i = 0; i < 8; ++i) {
        isValidAccount[i + 65] = 1;
    }
}


void error(char *msg) {
    err(-1, "%s", msg);
}

#endif //CLIENTSERVER_COMM_H