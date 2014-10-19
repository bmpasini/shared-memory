#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSIZE 1024

int main()
{
	key_t key;
	int shmid;
	int cnt;
	int infinite_loop = 1;
    char *shm, *s;
    char line[BUFSIZ];
    FILE * digits;

    digits = fopen ("digits.out","w");

    // We need to get the segment with key equal to the user ID, which was created by receiver.c
    key = getuid();
    
    // Locate the segment
    shmid = shmget(key, SHMSIZE, 0666);

    // Raise error if segment isn't located
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
    	if (*shm != '@') {

    		cnt = 0;
    		memset(line, 0, sizeof(line));

		    for (s = shm; *s != '$'; s++) {
		        line[cnt] = *s;
		        cnt++;
		    }

		    if (strncmp(line, "quit", 4) == 0) {
            	infinite_loop = 0;
        	}

		    *shm = '@';

		    // printf("%s\n", line);
		    
		    digits = fopen ("digits.out","a");

		    if (digits != NULL)
			{
				fprintf(digits, "%d: ", cnt-1);
				fputs(line, digits);
				fclose(digits);
			}
		}
	}

	if (shmdt(shm) == -1) {
	    perror("shmdt failed: segment wasn't dettached from data space");
	    exit(1);
	}
}