#include <semaphore.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "comm.h"

#define SEM_MUTEX "mutex"
#define SEM_NOTIFY "notify"
#define SHARED_MEM "mem"
#define SEM_BANK "bank"


struct BankSystem {
    sem_t *notify_sem;
    sem_t *bank_sem;
    sem_t *mutex;
    int fd_shm;
    struct User *user;
};


int open_shm(int flag, mode_t mode) {
    int fd_shm;

    if ((fd_shm = shm_open(SHARED_MEM, flag, mode)) == -1) {
        error("shm_open");
    }

    if (ftruncate(fd_shm, sizeof(struct User)) == -1) {
        error("Error while setting shared memory!");
    }

    return fd_shm;
}


sem_t *open_sem(char *name, int flag, mode_t modes, int value) {

    sem_t *sem;

    if (modes == 0000 || value == -1) {
        if ((sem = sem_open(name, flag)) == SEM_FAILED) {
            error("Error while opening semaphore.");
        }
    } else {
        if ((sem = sem_open(name, flag, modes, value)) == SEM_FAILED) {
            error("Error while opening semaphore.");
        }
    }

    return sem;
}

void post_sem(sem_t *sem, char *name) {
    if (sem_post(sem) == -1) {
        error(strcat("Error while posting sem: ", name));
    }
}

void wait_sem(sem_t *sem, char *name) {
    if (sem_wait(sem) == -1) {
        error(strcat("Error while waiting sem: ", name));
    }
}

struct User *map(int fd_shm) {
    struct User *user;

    if ((user = mmap(NULL, sizeof(struct User), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0)) == MAP_FAILED)
        error("Error while setting shared memory!");

    return user;
}

void initClientSemaphore(struct BankSystem *semaphore) {

    semaphore->mutex = open_sem(SEM_MUTEX, O_RDWR, 0000, -1);

    wait_sem(semaphore->mutex, "mutex");

    semaphore->fd_shm = open_shm(O_RDWR, 0);

    if (ftruncate(semaphore->fd_shm, sizeof(struct User)) < 0)
        error("Error while setting shared memory!");

    semaphore->user = map(semaphore->fd_shm);

    semaphore->notify_sem = open_sem(SEM_NOTIFY, O_RDWR, 0000, -1);

    semaphore->bank_sem = open_sem(SEM_BANK, O_RDWR, 0000, -1);
}

void initServerSemaphore(struct BankSystem *semaphore) {


    if ((semaphore->mutex = sem_open(SEM_MUTEX, O_CREAT, 0660, 0)) == SEM_FAILED) {
        error("Error while opening semaphore.");
    }

    sem_init(semaphore->mutex, 1, 0);

    semaphore->fd_shm = open_shm(O_RDWR | O_CREAT, 0660);

    if (ftruncate(semaphore->fd_shm, sizeof(struct User)) < 0)
        error("Error while setting shared memory!");

    semaphore->user = map(semaphore->fd_shm);

    semaphore->notify_sem = open_sem(SEM_NOTIFY, O_CREAT, 0660, 0);

    sem_init(semaphore->notify_sem, 1, 0);

    semaphore->bank_sem = open_sem(SEM_BANK, O_CREAT, 0660, 0);

    sem_init(semaphore->bank_sem, 1, 0);

    post_sem(semaphore->mutex, "mutex");
}