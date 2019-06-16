#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "semaphore.c"
#include "file_handler.c"

#define true 1

void validateArguments(int argc) {
    if (argc != 2) {
        error("You need to provide only the name of the file to read data from!");
    }
}

int createEmptyFile(char *fileName, uint32_t* bank) {

    int fd = open(fileName, O_CREAT | O_RDWR, CREATE_MODE);
    for (int i = 0; i < 8; ++i) {
        uint32_t temp = 0;
        uint32_t size = write(fd, &temp, 4);
        if (size != 4) {
            error("Could not write the numbers to the file.");
        }
        bank[i] = 0;

    }

    return fd;
}


int main(int argc, char *argv[]) {

    validateArguments(argc);

    initValidAccount();

    uint32_t bank[8];
    int fd;

    char *fileName = argv[1];
    if (!fileExist(fileName)) {
        fd = createEmptyFile(fileName, bank);
    } else {
        fd = open(fileName, O_RDWR);
        int bytesRead = read(fd, &bank, 8 * 4);
        if (bytesRead != 8 * 4)
            error("Could not read the numbers from the file");
    }

    struct Semaphore semaphore;

    char buff[256];

    initSemaphore(&semaphore, O_CREAT, CREATE_MODE, O_RDWR | O_CREAT);


    post_sem(semaphore.mutex_sem, "mutex_sem");

    while (true) {
        wait_sem(semaphore.spool_signal_sem, "spool_signal_sem");

        strcpy(buff, semaphore.shared_mem_ptr);
        char account = buff[1];
        uint8_t indexForValid = (uint8_t) account;
        uint8_t index = account - 65;

        if (!isValidAccount[indexForValid]) {
            sprintf(buff, "%d", -1);
            strcpy(semaphore.shared_mem_ptr, buff);

            post_sem(semaphore.take_from_bank_sem, "take_from_bank_sem");

            continue;
        } else {
            sprintf(buff, "%d", bank[index]);
            strcpy(semaphore.shared_mem_ptr, buff);
        }

        post_sem(semaphore.take_from_bank_sem, "take_from_bank_sem");

        wait_sem(semaphore.spool_signal_sem, "spool_signal_sem");

        long mult = 1;
        if (semaphore.shared_mem_ptr[0] == '-') {
            semaphore.shared_mem_ptr++;
            mult = -1;
        }

        long toAdd = strtol(semaphore.shared_mem_ptr, NULL, 10);
        if (toAdd > UINT16_MAX) {
            sprintf(buff, "%d", -1);
            strcpy(semaphore.shared_mem_ptr, buff);

            post_sem(semaphore.take_from_bank_sem, "take_from_bank_sem");

            continue;
        } else {
            toAdd *= mult;
            if ((long) bank[index] + toAdd < 0 || (long) bank[index] + toAdd > UINT32_MAX) {
                sprintf(buff, "%d", -1);
                strcpy(semaphore.shared_mem_ptr, buff);

                post_sem(semaphore.take_from_bank_sem, "take_from_bank_sem");

                continue;
            } else {
                bank[index] += toAdd;
                sprintf(semaphore.shared_mem_ptr, "%s", "success");
                lseek(fd, 0, SEEK_SET);
                int size = write(fd, &bank, 8 * 4);
                if (size != 8 * 4) {
                    error("Failed to write to file");
                }
            }
        }
        post_sem(semaphore.take_from_bank_sem, "take_from_bank_sem");
    }
}