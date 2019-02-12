// Christian Winds
// CSC 460
// Wednesday, February 6th, 2019
// Popo and Nana
// Program Description: This program creates double the number of processes
// 			specified in command line input; the processes
// 			synchronize to print a sequence of "Popo 1", "Nana 1",
// 			"Popo 2", "Nana 2", "Popo 3", and so forth five times

#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>

struct structPrintInfo
{
	char	name[5];
	int	processNumber;
};

int main(int argc, char *argv[])
{
	// Check to ensure a command line argument was entered in the command
	// line
	if (!(argc > 1))
	{
		printf("Please enter a number from 1 to 100.\n");
		return(0);
	}

        // Create an integer variable to use in calculating the total number of
        // processes to be formed
        int N = atoi(argv[1]);

	// Ensure the variable N is between one and one hundred
	if ((N < 1) || (N > 100))
	{
		printf("Please enter a number from 1 to 100.\n");
		return(0);
	}

	// Calculate the total number of processes to form
	//int maximumProcesses = 2 * N;

	// Create a char array to hold the name associated with the process
	char *processName = "Popo"; // * NOTE: May need correction	

	// Create an integer variable to track the process's designated number
	int processNumber = 1;

	// Create a struct to hold this process's information
	struct structPrintInfo processInfo;

	// Place this process's name into this process's process info struct
	strcpy(processInfo.name, processName);

	// Place this process's number into this process's process info struct
	processInfo.processNumber = processNumber;

	// Create shared memory for the processes to access
	int sharedMemoryId = shmget(IPC_PRIVATE, sizeof(struct structPrintInfo), 0764);
	if (sharedMemoryId == -1)
	{
		// Print an error message if shared memory could not be obtained
		printf("No shared memory could be retrieved.\n");
		return(0);
	}

	printf("Shared memory created.\n"); // * DEBUG

	// Write the information for the first printing process to shared memory
	struct structPrintInfo *ram;
	ram = (struct structPrintInfo *) shmat(sharedMemoryId, NULL, SHM_RND);
	strcpy(ram -> name, "Popo");
	ram -> processNumber = 1;

	// Create an integer of the number of times each process should print
	int printsRemaining = 5;

	// Create an integer of the number of created processes
	int createdProcesses = 1;

	// Initialize the parent process's child process ID to negative one
	int pidOfProcessChild = -1;

	// Create a struct with the information of the process to print after
	// the current process
	struct structPrintInfo nextProcessInfo;
	strcpy(nextProcessInfo.name, "Nana");
	nextProcessInfo.processNumber = 1;

	// Create an int variable for the number of the process that prints
	// after the current process
	int nextProcessNumber = processNumber;

	// Create the requested number of processes
	int processNumberToAssign = 2;
	while ((createdProcesses < N) && (pidOfProcessChild != 0))
	{
		pidOfProcessChild = fork();
		createdProcesses++;

		// Prepare the child process's variables
		if (pidOfProcessChild == 0)
		{
			// Set this child process's own process number
			processInfo.processNumber = processNumberToAssign;
		}
		else
		{
			// Increment the process number to assign
			processNumberToAssign++;
		}
	}

	// Create separate "Popo" and "Nana" processes
	pidOfProcessChild = fork();
	if (pidOfProcessChild != 0)
		strcpy(processInfo.name, "Popo");
	else
		strcpy(processInfo.name, "Nana");

	// Set the process name struct information for the process to print after
	// the current process
	if ((strcmp(processInfo.name, "Popo")) == 0)
		strcpy(nextProcessInfo.name, "Nana");
	else
		strcpy(nextProcessInfo.name, "Popo");

	// Set the process number struct information for the process to print
	// after the current process
	if ((strcmp(processInfo.name, "Popo")) == 0)
		nextProcessInfo.processNumber = processInfo.processNumber;
	else if (((strcmp(processInfo.name, "Nana")) == 0) &&
		 (processInfo.processNumber == N))
		nextProcessInfo.processNumber = 1;
	else
		nextProcessInfo.processNumber = processInfo.processNumber + 1;

	// Print each process's name and number in sequential order five times
	while (printsRemaining > 0)
	{
		// View the program's shared memory to determine whether to
		// print

		if (((strcmp(ram -> name, processInfo.name)) == 0) &&
		    (ram -> processNumber == processInfo.processNumber))
		{
			// Print this process's name and number
			printf("%s %d\n",
			       processInfo.name,
			       processInfo.processNumber);

			// Reduce the number of remaining prints for this process
			printsRemaining--;

			// Set shared memory with the information for the next
			// printing process
			strcpy(ram -> name, nextProcessInfo.name);
			ram -> processNumber = nextProcessInfo.processNumber;
		}
	}

	// If this process was the final process to print, clean the shared
	// memory
	if (((strcmp(processInfo.name, "Nana")) == 0) &&
	    (processInfo.processNumber == N))
	{
		// Instruct the final printing process to sleep to ensure shared
		// memory remains availale for sufficient time
		sleep(10);

		if (((strcmp(ram -> name, "Popo")) == 0) &&
                    (ram -> processNumber == 1))
		{
			if ((shmctl(sharedMemoryId, IPC_RMID, NULL)) == -1)
			{
				// Print an error message if the shared memory
				// to remove was not found
				printf("The shared memory could not be removed.\n");
			}
		}
	}

	// If this process is the original process, sleep to ensure all output
	// is formatted correctly
	if (((strcmp(processInfo.name, "Popo")) == 0) &&
            (processInfo.processNumber == 1))
	{
		sleep(6);
	}

	return 0;
}
