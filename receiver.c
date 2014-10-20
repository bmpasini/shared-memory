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
	// ****************************************************
	// Variables declaration
	// ****************************************************

	key_t key;
	int shmid;
	int infinite_loop = 1;
	char *shm, *s, c;
	char *user_input = malloc(sizeof(char)*BUFSIZ);

	// ****************************************************
	// Setup of shared memory
	// ****************************************************

	// The key of the shared memory segment will be the user ID
	key = getuid();

	// The shared memory segment is created
	shmid = shmget(key, SHMSIZ, IPC_CREAT | 0666);

	// Error is raised if segment isn't created
	if (shmid < 0) {
		perror("shmget failed: shared memory segment wasn't created");
		exit(EXIT_FAILURE);
	}
	
	// Shared memory segment is attached to data space
	shm = shmat(shmid, NULL, 0);

	// Error is raised if segment isn't attached
	if (shm == (char *) -1) {
		perror("shmat failed: segment wasn't attached to data space");
		exit(EXIT_FAILURE);
	}

	// ****************************************************
	// User enters input in an infinite loop
	// ****************************************************

	while (infinite_loop) {

		// User is asked for an alpha numeric string input
		printf("Enter an alpha numeric string: ");
		fgets(user_input, BUFSIZ, stdin);

		// If the input string is "quit" the infinite loop is interrupted, so that the shared memory segment can be dettached and the IPC can be closed
		if (strncmp(user_input, "quit", 4) == 0) {
			infinite_loop = 0;
		}

		// User input is copied to shared memory
		memcpy(shm, user_input, strlen(user_input));

		// A dollar sign '$' is added to the end of the input string, so that processor.c knows when the input ends ($ is not alpha numeric, so it won't harm the program main functionality)
		s = shm;
		s += strlen(user_input);
		*s = '$';

		// Processor.c signalizes it has finished reading the input string by inserting an '@' symbol to the memory address shm is pointing to (the beginning of the shared memory string). So, here receiver.c is waiting for that signal so that it can restart the loop. ('@' is not alpha numeric, so it won't harm the program main functionality)
		while (*shm != '@')
			sleep(1);
	}

	// ****************************************************
	// Shared memory communication has to close
	// ****************************************************

	// The shared memory segment should be dettached from data space. An error is raised if segment isn't dettached
	if (shmdt(shm) == -1) {
		perror("shmdt failed: segment wasn't dettached from data space");
		exit(1);
	}

	// The shared memory IPC communication should be closed. An error is raised if communication doesn't close
	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		perror("shmctl failed: ipc didn't close");
		exit(1);
	}
}