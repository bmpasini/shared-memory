#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSIZ 1024

int main(void)
{
	key_t key;
    int shmid;
    int infinite_loop = 1;
    char *shm, *s, c;
    char *user_input = malloc(sizeof(char)*BUFSIZ);

    // The key of the shared memory segment will be the user ID
    key = getuid();
    
    // Creation of shared memory segment
    shmid = shmget(key, SHMSIZ, IPC_CREAT | 0666);

    // Raise error if segment isn't created
    if (shmid < 0) {
        perror("shmget failed: shared memory segment wasn't created");
        exit(EXIT_FAILURE);
    }
    
    // Attach the segment to data space
    shm = shmat(shmid, NULL, 0);

    // Raise error if segment isn't attached
    if (shm == (char *) -1) {
        perror("shmat failed: segment wasn't attached to data space");
        exit(EXIT_FAILURE);
    }

    while (infinite_loop) {

	    printf("Enter an alpha numeric string: ");
	    fgets(user_input, BUFSIZ, stdin);

	    if (strncmp(user_input, "quit", 4) == 0) {
            infinite_loop = 0;
        }

        memcpy(shm, user_input, strlen(user_input));

	    s = shm;
        s += strlen(user_input);
        *s = '$';

        while (*shm != '@')
            sleep(1);
	}

	if (shmdt(shm) == -1) {
	    perror("shmdt failed: segment wasn't dettached from data space");
	    exit(1);
	}

	if (shmctl(shmid, IPC_RMID, 0) == -1) {
	    perror("shmctl failed: ipc wasn't cleared");
	    exit(1);
	}
}