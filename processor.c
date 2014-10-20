#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSIZE 1024

int main(void)
{
	// ****************************************************
	// Variables declaration
	// ****************************************************

	key_t key;
	int shmid;
	int cnt;
	int infinite_loop = 1;
	char *shm, *s;
	char line[BUFSIZ];
	FILE * digits;

	// ****************************************************
	// Output file 'digits.out' created
	// ****************************************************

	digits = fopen("digits.out","w");

	// ****************************************************
	// Setup of shared memory
	// ****************************************************

	// We need to get the segment with key equal to the user ID, which was created by receiver.c
	key = getuid();

	// Shared memory segment is located
	shmid = shmget(key, SHMSIZE, 0666);

	// Error is raised if segment isn't located
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
	// User input is received and written into output file
	// in an infinite loop
	// ****************************************************

	while (infinite_loop) {

		// Program waits for user input so that it can run properly
		if (*shm != (int) NULL) {

			// *shm begins with an '@' while there is no new input. Thus, the program has to wait for a new input
			if (*shm != '@') {

				// Digits counter is set to zero and the line input string is emptied for being able to receive new input values
				cnt = 0;
				memset(line, 0, sizeof(line));

				// The string received through shared memory is passed into a line characters array, and also it counts the number of characters in the input string
				for (s = shm; *s != '$'; s++) {
					line[cnt] = *s;
					cnt++;
				}

				// If the input string is "quit" the infinite loop is interrupted, so that the shared memory segment can be dettached
				if (strncmp(line, "quit", 4) == 0) {
					infinite_loop = 0;
				}

				// The program signalizes it has finished reading the input string by inserting an '@' symbol to the memory address shm is pointing to (the beginning of the shared memory string). Thus, receiver.c can ask the user for a new input and also, process.c is signalized to wait for the new input before writing it into the output file
				*shm = '@';
				
				// The 'digits.out' file is opened so that new text can be appended into it
				digits = fopen ("digits.out","a");

				// The number of characters in the input string is written in the output 'digits.out' file, along with the input string. Also, the output file is closed
				if (digits != NULL)
				{
					fprintf(digits, "%d: ", cnt-1);
					fputs(line, digits);
					fclose(digits);
				}
			}
		}
	}

	// ****************************************************
	// Shared memory has to be dettached from data space
	// ****************************************************

	// The shared memory segment should be dettached from data space. An error is raised if segment isn't dettached
	if (shmdt(shm) == -1) {
		perror("shmdt failed: segment wasn't dettached from data space");
		exit(1);
	}
}