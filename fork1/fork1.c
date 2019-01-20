// Christian Winds
// CSC 460
// Sunday, January 20th, 2019
// Fork I - C program

#include <stdio.h>

int main(int argc, char * argv[])
{
	// Crate an integer variable for the maximum number of children one
	// process generation's processes should each create
	int N;

	// Set the maximum number of children a process should create
	N = atoi(argv[1]);

	// End the program if the user entered a value less than zero or greater
	// than five
	if ((N < 0) || (N > 5))
	{
		// Display an error message and end the program
		printf("Please enter an integer from 0 to 5.\n");
		return 0;
	}

	int pidOfProcessChild;

	// Create an integer variable to track each process's generation number
	int generationNumber = 0;

	// Calculate the number of children the current process should create
	int childrenToCreate = generationNumber + 1;

	// Print a header line for the process information table
	printf("Gen\tPID\tPPID\n");

	// Create new process generations until each of one generation's
	// processes creates N children
	while (childrenToCreate > 0)
	{
		// Create no more children if this process would create more
		// than N children
		if (childrenToCreate > N)
			break;
		else
		{
			// Create a process for the next process generation
			pidOfProcessChild = fork();

			// If the process that reached this if statement is a
			// new child process, increment that child's generation
			// number by one and set a new number of children to
			// create. If not, decrement the number of children to
			// create.
			if (pidOfProcessChild == 0)
			{
				generationNumber++;
				childrenToCreate = generationNumber + 1;
			}
			else
				childrenToCreate--;
		}
	}

	// Print the current process's generation, process ID, and parent
	// process ID
	printf("%d\t%d\t%d\n",generationNumber,getpid(),getppid());

	// Ensure each parent's process ID is available when needed by
	// instructing each process to sleep
	sleep(2);

	return 0;
}
