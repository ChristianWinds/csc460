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
	int maximumProcesses = 2 * N;

	// Create an integer variable to track the process's designated number
	int processNumber = 1;

	// Convert the process number integer variable to a char string
	char *processNumberChar;
	sprintf(processNumberChar, "%d", processNumber);

	// Create a char pointer to point toward the process number
	char *processNumberPointer = (char*)processNumberChar;

	// Create a char array to hold the name associated with the process
	char *processName = "Popo"; // * NOTE: May need correction	

	// Create a char string holding the line for the process to print
	char *processPrintLine = strcat(processName, processNumberPointer); // * NOTE: May need correction

	// Create shared memory for the processes to access
	int sharedMemoryId = shmget(IPC_PRIVATE, 7*sizeof(char), 0764);
	if (sharedMemoryId == -1)
	{
		// Print an error message if shared memory could not be obtained
		printf("No shared memory could be retrieved.\n");
		return(0);
	}

	// Write the print line for the first printing process to shared memory
	char *ram;
	ram = (char *) shmat(sharedMemoryId, NULL, SHM_RND);
	strcpy(ram, processPrintLine);

	// Create an integer of the number of times each process should print
	int printsRemaining = 5;

	// Create an integer of the number of created processes
	int createdProcesses = 1;

	// Initialize the parent process's child process ID to zero
	int pidOfProcessChild = 0;

	// Create a char string of the name of the process that prints after the
	// current process
	char *nextProcessName = "Nana"; // * NOTE: May need to be corrected

	// Create an int variable for the number of the process that prints after
	// the current process
	int nextProcessNumber = processNumber;

	// Convert the next process number integer variable to a char string
	char *nextProcessNumberChar;
	sprintf(nextProcessNumberChar, "%d", nextProcessNumber);

	// Create a char pointer to point toward the process number
	char *nextProcessNumberPointer = (char*)nextProcessNumberChar;

	// Create a char string to hold the instruction for which process begins
	// printing after the current process
	char nextProcessPrintLine = strcat(nextProcessName, (char*)nextProcessNumberPointer); // * NOTE: May need to be corrected

	// Create the requested number of processes
	while ((createdProcesses < maximumProcesses) && (pidOfProcessChild == 0))
	{
		pidOfProcessChild = fork();
		createdProcesses++;

		// Prepare the child process's variables
		if (pidOfProcessChild == 0)
		{
			// Set this child process's own process information
			processName = nextProcessName;
			processNumber = nextProcessNumber;

			// Prepare the information for which process should
			// print after this child process
			if (processName == "Popo")
			{
				// Set the next process as a Nana process
				nextProcessName = "Nana";
			}
			else
			{
				// Set the next process as a Popo process
				nextProcessName = "Popo";
				if (createdProcesses == maximumProcesses)
				{
					// Set the next process number to one to
					// restart the process printing sequence
					nextProcessNumber = 1;
				}
				else
					nextProcessNumber++;
			}

			// Place the decided next process's print line into the
			// nextProcessPrintLine char string
			nextProcessPrintLine = (nextProcessName, nextProcessNumberPointer); // * NOTE: May need to be corrected
		}
	}

	// Print each process's name and number in sequential order five times
	while (printsRemaining > 0)
	{
		// View the program's shared memory to determine whether to
		// print
		ram = (char *) shmat(sharedMemoryId, NULL, SHM_RND);

		if (ram == processPrintLine)
		{
			// Print this process's name and number
			printf("%s %d", processName, processNumber); // * NOTE: This line may need to be corrected

			// Reduce the number of remaining prints for this process
			printsRemaining--;

			// Set shared memory with the information for the next
			// printing process
			strcpy(ram, nextProcessPrintLine); // * NOTE: Refer to create.c
		}
	}

	// If this process was the final process to print, clean the shared
	// memory
	if ((processName == "Nana") && (processNumber == N))
	{
		if ((shmctl(sharedMemoryId, IPC_RMID, NULL)) == -1)
		{
			// Print an error message if the shared memory to remove
			// was not found
			printf("The shared memory could not be removed.\n");
		}
	}

	return 0;
}
