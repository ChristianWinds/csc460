// Christian Winds
// CSC 460
// Friday, March 1st, 2019
// HW8 - Kirby's Deadly Diner
// Program Description: This program uses a poor solution to the Dining
// 			Philosophers problem; the program eventually deadlocks

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

int debug = 0;

// Set the number of chopsticks
const int CHOPSTICKS = 5;

// Set the maximum number of dinner guests
const int MAXDINNERGUESTS = 5;

void think(int guestNumber);
void hunger(int guestNumber, int leftChopstick, int rightChopstick, int chopstickSemId);
void eat(int guestNumber, int leftChopstick, int rightChopstick, int chopstickSemId);
void eatCpuCycles();

int main()
{
	// Create an array of five semaphores to hold five total chopsticks
	int chopstickSemId = semget (IPC_PRIVATE, CHOPSTICKS, 0777);

	// Ensure the chopsticks semaphore was created properly
	if (chopstickSemId == -1)
	{
		// Report that the chopsticks semaphore creation had not
		// succeeded, then end the program
		printf("The chopsticks semaphore could not be created ");
		printf("correctly.\n");
		return (-1);
	}


	// Initialize each of the chopstick semaphores
	int semaphoreIndex;

	if (debug)
	{
		printf("* Entering semaphores initialization loop.\n");
	}

	for (semaphoreIndex = 0; semaphoreIndex < CHOPSTICKS; semaphoreIndex++)
	{
		// Initialize the current semaphore
		semctl(chopstickSemId, semaphoreIndex, SETVAL, 1);
	}

	if (debug)
	{
		printf("* All chopstick semaphores initialized.\n");
	}

	// Create an integer variable of this process's guest number
	int guestNumber = 0;

	// Create an integer variable to hold each process's child's process ID
	int processIdOfChild = -1;

	// Create an integer variable to track the total number of existing
	// processes
	int totalProcesses = 1;

	if (debug)
	{
		printf("* Pre-process creation while loop.\n");
	}

	// Create four additional processes
	while (totalProcesses < MAXDINNERGUESTS)
	{
		// Create a new process
		processIdOfChild = fork();

		totalProcesses++;

		// If the current process is a child process, modify the
		// process's guest number and leave the process creation loop
		if (processIdOfChild == 0)
		{
			// Change the process's guest number
			guestNumber = totalProcesses - 1;

			break;
		}
	}

	if (debug)
	{
		printf("* Process creation while loop completed; processIdOfChild: %d\n", processIdOfChild);
	}

	// Determine the semaphores corresponding to this process's left and
	// right chopsticks
	int leftChopstick = guestNumber;
	int rightChopstick = (guestNumber + 1) % MAXDINNERGUESTS;

	if (debug)
	{
		printf("* Left and right chopsticks determined; processIdOfChild: %d\n", processIdOfChild);
	}

	// Enter an infinite loop of the Dining Philosophers problem
	while (1)
	{
		if (debug)
		{
			printf("* Infinite while loop iterated; processIdOfChild: %d; guestNumber: %d\n", processIdOfChild, guestNumber);
		}

		think(guestNumber);

		if (debug)
		{
			printf("* think process completed; processIdOfChild: %d; guestNumber: %d\n", processIdOfChild, guestNumber);
		}

		hunger(guestNumber, leftChopstick, rightChopstick, chopstickSemId);

		if (debug)
		{
			printf("* hunger process completed; processIdOfChild: %d; guestNumber: %d\n", processIdOfChild, guestNumber);
		}

		eat(guestNumber, leftChopstick, rightChopstick, chopstickSemId);

		if (debug)
		{
			printf("* eat process completed; processIdOfChild: %d; guestNumber: %d\n", processIdOfChild, guestNumber);
		}

	}

	return 0;
}

void think(int guestNumber)
{
	// Postcondition: This process will have printed to the screen "(this
	// 		  process's number) THINKING" in an on-screen text
	// 		  column that corresponds to this process's guest
	// 		  number; several CPU cycles will also have been used

	if (debug)
	{
		printf("* think(): think processs entered; guestNumber: %d\n", guestNumber);
	}

	// Print a number of tabs matching this process's guest number
	int tabsPrinted;
	for (tabsPrinted = 0; tabsPrinted < guestNumber; tabsPrinted++)
	{
		printf("\t");
	}
	
	// Print this process's "(guest number) THINKING" line
	printf("%d THINKING", guestNumber);

	if (debug)
	{
		printf("* think(): About to call eatCpuCycles; guestNumber: %d\n", guestNumber);
	}

	// Use several CPU cycles
	/*eatCpuCycles();*/ // * DEBUG: Temporary disable
	int outerLoopsRun;
	int innerLoopsRun;
	int maximumLoops = 20000;
	for (outerLoopsRun = 0; outerLoopsRun < maximumLoops; outerLoopsRun++)
	{
		for (innerLoopsRun = 0; innerLoopsRun < maximumLoops; innerLoopsRun++)
		{
			;
		}
	}

	if (debug)
	{
		printf("* think(): think processs ending; guestNumber: %d\n", guestNumber);
	}
}

void hunger(int guestNumber, int leftChopstick, int rightChopstick, int chopstickSemId)
{
	
	// Postcondition: This process will have printed to the screen "(this
	// 		  process's number) HUNGRY" in an on-screen text
	// 		  column that corresponds to this process's guest
	// 		  number; the process will also have acquired the
	// 		  chopsticks at this process's left and right

	// Print a number of tabs matching this process's guest number
	int tabsPrinted;
	for (tabsPrinted = 0; tabsPrinted < guestNumber; tabsPrinted++)
	{
		printf("\t");
	}
	
	// Print this process's "(guest number) HUNGRY" line
	printf("%d HUNGRY", guestNumber);

	// Acquire this process's left chopstick
	p(leftChopstick, chopstickSemId);

	// Acquire this process's right chopstick
	p(rightChopstick, chopstickSemId);
}

void eat(int guestNumber, int leftChopstick, int rightChopstick, int chopstickSemId)
{
	// Postcondition: This process will have printed to the screen "(this
	// 		  process's number) EATING" in an on-screen text
	// 		  column that corresponds to this process's guest
	// 		  number; the process will also have used several CPU
	// 		  cycles, and have placed the process's respective left
	// 		  and right chopsticks back within the chopsticks
	// 		  semaphore

	// Print a number of tabs matching this process's guest number
	int tabsPrinted;
	for (tabsPrinted = 0; tabsPrinted < guestNumber; tabsPrinted++)
	{
		printf("\t");
	}
	
	// Print this process's "(guest number) EATING" line
	printf("%d EATING", guestNumber);

	// Use several CPU cycles
	/*eatCpuCycles();*/ // * DEBUG: Temporary disable
	int outerLoopsRun;
	int innerLoopsRun;
	int maximumLoops = 20000;
	for (outerLoopsRun = 0; outerLoopsRun < maximumLoops; outerLoopsRun++)
	{
		for (innerLoopsRun = 0; innerLoopsRun < maximumLoops; innerLoopsRun++)
		{
			;
		}
	}

	// Place the left chopstick back into the chopsticks semaphore
	v(rightChopstick, chopstickSemId);

	// Place the right chopstick back into the chopsticks semaphore
	v(rightChopstick, chopstickSemId);
}

/*void eatCpuCycles()
{
	// Postcondition: Several CPU cycles were used

	// Create integer variables to track the numbers of loop iterations run
	int numberOfOuterLoopsRun;
	int numberOfInnerLoopsRun;

	// Prepare the random number generator
	time_t timeVariable;
	srand((unsigned) time(&timeVariable));

	// Create integer variables to determine the maximum number of times
	// each loop runs
	int maximumOuterLoops = (rand() % 10000) + 1000;
	int maximumInnerLoops = (rand() % 10000) + 1000;

	// Run the determined numbers of loops
	for (numberOfOuterLoopsRun = 0;
	     numberOfOuterLoopsRun < maximumOuterLoops;
	     numberOfOuterLoopsRun++)
	{
		for (numberOfInnerLoopsRun = 0;
		     numberOfInnerLoopsRun < maximumInnerLoops;
		     numberOfInnerLoopsRun++);
	}
}*/ // * DEBUG: Temporary disable

p(int s,int sem_id)
{
struct sembuf sops;

sops.sem_num = s;
sops.sem_op = -1;
sops.sem_flg = 0;
if((semop(sem_id, &sops, 1)) == -1) printf("%s", "'P' error\n");
}

v(int s, int sem_id)
{
struct sembuf sops;

sops.sem_num = s;
sops.sem_op = 1;
sops.sem_flg = 0;
if((semop(sem_id, &sops, 1)) == -1) printf("%s","'V' error\n");
}
