#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSIZE 1024

// Setup Shared Memory
char* setupSharedMemory(void)
{
    int shmid;
    key_t key;
    char *shm;
    
    // We need to get the segment with key "2727", created by receiver.c
    key = 2727;
    
    // Locate the segment
    shmid = shmget(key, SHMSIZE, 0666);

    // Raise error if segment isn't located
    if (shmid < 0) {
        perror("shmget failed: shared memory segment wasn't created");
        exit(1);
    }
    
    // Attach the segment to data space
    shm = shmat(shmid, NULL, 0);

    // Raise error if segment isn't attached
    if (shm == (char *) -1) {
        perror("shmat failed: segment wasn't attached to data space");
        exit(1);
    }

    return shm;
}


int main()
{
    char *shm, *s;
    
    shm = setupSharedMemory();

    /*
     * Now read what the server put in the memory.
     */
    for (s = shm; *s != 0; s++)
        putchar(*s);
    putchar('\n');
    
    /*
     * Finally, change the first character of the
     * segment to '*', indicating we have read
     * the segment.
     */
    *shm = '*';
    
    exit(0);
}
