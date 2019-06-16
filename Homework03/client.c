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

    semaphore.mutex_sem = open_sem(SEM_MUTEX_NAME, 0, 0, 0);

    semaphore.fd_shm = open_shm(O_RDWR, 0);

    semaphore.shared_mem_ptr = mmap(NULL, 256, PROT_READ | PROT_WRITE, MAP_SHARED, semaphore.fd_shm, 0);

    // counting semaphore, indicating the number of strings to be printed. Initial value = 0
    semaphore.spool_signal_sem = open_sem(SEM_SPOOL_SIGNAL_NAME, 0, 0, 0);

    semaphore.take_from_bank_sem = open_sem(SEM_BANK_NAME, O_CREAT, CREATE_MODE, 0);

    wait_sem(semaphore.mutex_sem, "mutex_sem");

    sprintf(semaphore.shared_mem_ptr, "%s", argv[1]);

    // Tell spooler that there is a string to print: V (spool_signal_sem);
    post_sem(semaphore.spool_signal_sem, "spool_signal_sem");

    wait_sem(semaphore.take_from_bank_sem, "take_from_bank");

    if (semaphore.shared_mem_ptr[0] == '-') {
        // Release mutex sem: V (mutex_sem)
        post_sem(semaphore.mutex_sem, "mutex_sem");

        munmap(semaphore.shared_mem_ptr, 256);
        error("Invalid account number!");
    } else {
        printf("Account %s has %s left in it. What transaction would you like to make:\n", argv[1], semaphore.shared_mem_ptr);
    }

    char buff[16];
    if (fgets(buff, 16, stdin) == NULL) {
        error("Failed to get the desired amount");
    }

    sprintf(semaphore.shared_mem_ptr, "%s", buff);

    post_sem(semaphore.spool_signal_sem, "spool_signal_sem");

    wait_sem(semaphore.take_from_bank_sem, "take_from_bank");

    if (semaphore.shared_mem_ptr[0] == '-') {
        // Release mutex sem: V (mutex_sem)
        post_sem(semaphore.mutex_sem, "mutex_sem");

        munmap(semaphore.shared_mem_ptr, 256);
        error("Invalid amount! Could not make the transaction");
    }

    // Release mutex sem: V (mutex_sem)
    post_sem(semaphore.mutex_sem, "mutex_sem");

    munmap(semaphore.shared_mem_ptr, 256);
    exit(0);
}