#include <semaphore.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <bits/fcntl-linux.h>
#include <unistd.h>
#include <string.h>
#include "comm.h"

int open_shm(int flag, mode_t mode) {
    int fd_shm;

    if ((fd_shm = shm_open(SHARED_MEM_NAME, flag, mode)) == -1) {
        error("shm_open");
    }

    if (ftruncate(fd_shm, 256) < 0) {
        error("Could not set the shared memory");
    }

    return fd_shm;
}

sem_t *open_sem(char *name, int flag, int mode, int value) {

    sem_t *mutex_sem;

    if ((mutex_sem = sem_open(name, flag, mode, value)) == SEM_FAILED) {
        error("sem_open failed.");
    }

    return mutex_sem;
}

void post_sem(sem_t *sem, char *r) {
    if (sem_post(sem) == -1) {
        error(strcat("sem_post:", r));
    }
}

void wait_sem(sem_t *sem, char *r) {
    if (sem_wait(sem) == -1) {
        error(strcat("sem_wait: ", r));
    }
}