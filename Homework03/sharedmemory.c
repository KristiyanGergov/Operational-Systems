#include <sys/param.h>
#include <sys/mman.h>
#include <unistd.h>
#include "comm.h"

#define SHARED_MEM_NAME "/posix-shared-mem-example"

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