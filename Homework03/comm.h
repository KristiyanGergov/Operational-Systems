#ifndef CLIENTSERVER_COMM_H
#define CLIENTSERVER_COMM_H

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define FIRST_ARGUMENT_INDEX 1

struct User {
    uint32_t balance;
    int32_t amount;
    char account;
    int accIndex;
    int result;
};

struct User createUser(char accountId) {
    struct User user;

    user.balance = 0;
    user.amount = 0;
    user.account = accountId;
    user.accIndex = accountId - 'A';

    return user;
}

void error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(-1);
}


void validateClientInput(int argc, char *argv[]) {

    if (argc != 2) {
        error("Invalid number of arguments! Please provide only the account ID.");
    }

    char id = argv[FIRST_ARGUMENT_INDEX][0];

    if (strlen(argv[FIRST_ARGUMENT_INDEX]) != 1 || id < 'A' || id > 'H') {
        error("The account identifier must match [A-H] and must be exactly one letter long.");
    }
}

void validateServerInput(int argc) {
    if (argc != 2) {
        error("Invalid number of arguments! Please provide only the file to read data from.");
    }
}

#endif //CLIENTSERVER_COMM_H