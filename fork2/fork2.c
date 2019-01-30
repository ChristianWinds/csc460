// Christian Winds
// CSC 460
// Wednesday, January 30th, 2019
// Fork 2 – Bad Synch program
// Program Description: This program creates one or more processes that
// 			synchronize to print an alphabetic sequence of letters
// 			five times.

#include <stdio.h>

int main(int argc, char * argv[])
{
	// Prepare pointers to use to access an external sync file
	FILE *fopen(), *filePointer;

	// Attempt to open the sync file for writing, ending the program if the
	// sync file cannot be found
	if ((filePointer = fopen("syncFile","w")) == NULL)
	{
		printf("The file syncFile could not be found.\n");
		return(0);
	}

	// Create an integer variable for the number of processes to create
	int N = atoi(argv[1]);

	// If an integer less than 1 or greater than 26 was chosen as N, print
	// an error message and end the program
	if ((N < 1) || (N > 26))
	{
		printf("Enter an integer from 1 to 26.\n");
		return 0;
	}

	// Create a char variable to hold the starting process letter
	char startingLetter = 'A';

	// Prepare the sync file to decide which process prints information
	// first
	fprintf(filePointer,"%c",startingLetter);
	fclose(filePointer);

	// Create a char variable that holds the process's letter designation;
	// set the variable with the first process's letter designation
	char letterDesignation = startingLetter;

	// Create a char variable that holds the letter process to print after
	// the current process prints
	char nextLetter;

	// Prepare the nextLetter variable with the letter designation of the
	// function that prints after the parent function
	if (N == 1)
	{
		// If this program's run lets only one process exist, match
		// nextLetter's letter to letterDesignation's letter
		nextLetter = letterDesignation;
	}
	else if (N > 1)
	{
		// If this program run creates at least one child process, set
		// nextLetter to the letter after letterDesignation
		nextLetter = letterDesignation;
		nextLetter++;
	}

	// Initialize the parent process's child process ID value to zero
	int pidOfChildProcess = 0;

	// Create the number of processes specified by N
	int existingProcesses = 1;
	while ((existingProcesses < N) && (pidOfChildProcess == 0))
	{
		pidOfChildProcess = fork();
		existingProcesses++;

		// Increment the letterDesignation of the child process, and
		// assign the correct next letter to the nextLetter variable
		if (pidOfChildProcess == 0)
		{
			letterDesignation++;
			if (existingProcesses == N)
				nextLetter = startingLetter;
			else
				nextLetter++;	
		}
	}

	// Set an integer variable of the number of remaining times this
	// function must print
	int timesLeftToPrint = 5;

	// Create a char variable to store the letter of the current process
	// permitted to print
	char permittedPrinter;

	// Until this process has printed the maximum number of times, print
	// this program's letter when the sync file permits printing
	while (timesLeftToPrint > 0)
	{
		// Attempt to open the sync file for reading, ending the program
		// if the sync file cannot be found
		if ((filePointer = fopen("syncFile","r")) == NULL)
		{
			printf("The file syncFile could not be found.\n");
			return 0;
		}

		// Read the sync file's letter for the process permitted to
		// print
		fscanf(filePointer,"%c",&permittedPrinter);
		fclose(filePointer);

		// If the scan file's held letter matches this process's
		// character designation, print this process's letter and
		// process ID, reduce the remaining times this process must
		// print, and prepare the sync file for the next process to 
		// print
		if (permittedPrinter == letterDesignation)
		{
			// Print this process's letter and process ID
			printf("%c:%d\n",letterDesignation,getpid());

			// Reduce this process's remaining times to print
			timesLeftToPrint--;

			// Attempt to open the sync file for writing, ending the
			// program if the sync file cannot be found
			if ((filePointer = fopen("syncFile","w")) == NULL)
			{
				printf("The file syncFile could not be found.\n");
				return(0);
			}

			// Change the sync file's letter to the next letter
			// designation to permit for printing
			fprintf(filePointer,"%c",nextLetter);
			fclose(filePointer);
		}
	}

	// Instruct the process to sleep to avert output format errors
	sleep(5);
	
	return 0;
}
