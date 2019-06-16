#include <semaphore.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "comm.h"

#define SEM_MUTEX_NAME "/sem-mutex"
#define SEM_SPOOL_SIGNAL_NAME "/sem-spool-signal"
#define SHARED_MEM_NAME "/posix-shared-mem-example"
#define SEM_BANK_NAME "/sem-bank"

struct Semaphore {
    char *shared_mem_ptr;
    sem_t *mutex_sem;
    sem_t *spool_signal_sem;
    sem_t *take_from_bank_sem;
    int fd_shm;
};


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

void initSemaphore(struct Semaphore *semaphore, int semFlags, int semMode, int shmFlag){

    semaphore->mutex_sem = open_sem(SEM_MUTEX_NAME, 0, semFlags, semMode);

    semaphore->fd_shm = open_shm(shmFlag, 0);

    semaphore->shared_mem_ptr = mmap(NULL, 256, PROT_READ | PROT_WRITE, MAP_SHARED, semaphore->fd_shm, 0);

    
    semaphore->spool_signal_sem = open_sem(SEM_SPOOL_SIGNAL_NAME, semFlags, semMode, 0);

    semaphore->take_from_bank_sem = open_sem(SEM_BANK_NAME, O_CREAT, CREATE_MODE, 0);
}