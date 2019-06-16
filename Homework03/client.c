#include <sys/types.h>
#include <fcntl.h>
#include "semaphore.c"
#include <sys/mman.h>

void validateArguments(int argc, char *argv[]) {
    if (argc != 2) {
        error("The client takes exactly one argument. Namely the bank identifier");
    }

    if (strlen(argv[1]) != 2) {
        error("Invalid id");
    }
}

int main(int argc, char *argv[]) {

    validateArguments(argc, argv);

    struct Semaphore semaphore;

    initSemaphore(&semaphore, 0, 0, O_RDWR);

    wait_sem(semaphore.mutex_sem, "mutex_sem");

    sprintf(semaphore.shared_mem_ptr, "%s", argv[1]);

    post_sem(semaphore.spool_signal_sem, "spool_signal_sem");

    wait_sem(semaphore.take_from_bank_sem, "take_from_bank");

    if (semaphore.shared_mem_ptr[0] == '-') {

        post_sem(semaphore.mutex_sem, "mutex_sem");

        munmap(semaphore.shared_mem_ptr, 256);
        error("Invalid account number!");
    } else {
        printf("Account %s has %s left in it. What transaction would you like to make:\n", argv[1],
               semaphore.shared_mem_ptr);
    }

    char buff[16];
    if (fgets(buff, 16, stdin) == NULL) {
        error("Failed to get the desired amount");
    }

    sprintf(semaphore.shared_mem_ptr, "%s", buff);

    post_sem(semaphore.spool_signal_sem, "spool_signal_sem");

    wait_sem(semaphore.take_from_bank_sem, "take_from_bank");

    if (semaphore.shared_mem_ptr[0] == '-') {

        post_sem(semaphore.mutex_sem, "mutex_sem");

        munmap(semaphore.shared_mem_ptr, 256);
        error("Invalid amount! Could not make the transaction");
    }

    post_sem(semaphore.mutex_sem, "mutex_sem");

    munmap(semaphore.shared_mem_ptr, 256);
    exit(0);
}