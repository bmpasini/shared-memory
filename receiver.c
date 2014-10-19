#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSIZ 1024

// Setup Shared Memory
char* setupSharedMemory(int shmsiz)
{
    int shmid;
    key_t key;
    char *shm;
    
    // The key of the shared memory segment will be "2727"
    key = 2727;
    
    // Creation of shared memory segment
    shmid = shmget(key, shmsiz, IPC_CREAT | 0666);

    // Raise error if segment isn't created
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

void endSharedMemory(char *shm, int shmsiz)
{
    int key = 2727;

    if (shmdt(shm) == -1) {
        perror("shmdt failed: segment wasn't dettached from data space");
        exit(1);
    }

    if (shmctl(shmget(key, shmsiz, 0666), IPC_RMID, 0) == -1) {
        perror("shmctl failed: ipc wasn't cleared");
        exit(1);
    }
}

char* getUserInput(void)
{
    char *str = malloc(sizeof(char)*BUFSIZ);

    printf("Enter an alpha numeric string: ");
    fgets(str, sizeof(str), stdin);

    return str;
}

int main(void)
{
    int user_input_size;
    char *shm, *s, c;
    char *user_input = malloc(sizeof(char)*BUFSIZ);

    // shm = setupSharedMemory(SHMSIZ);

    while (1) {
        strcpy(user_input, getUserInput());
        user_input_size = sizeof(user_input);

        shm = setupSharedMemory(user_input_size);

        // printf("%lu\n", strlen(user_input));

        s = shm;
        s += strlen(user_input);
        *s = 0;

        memcpy(shm, user_input, strlen(user_input));

        while (*shm != '*')
            sleep(1);

        endSharedMemory(shm, user_input_size);
    }
    
}
