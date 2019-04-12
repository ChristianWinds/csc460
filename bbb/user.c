// Christian Winds
// CSC 460
// Friday, April 12th, 2019
// HW10 - Big Bad Bowser's Dispatcher
// Program Description: This program simulates a user sending jobs to a CPU
// 			dispatcher

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>

struct jobInformation
{
	int processId;
	int cpuRequest;
	int initialClockTime;
	int neededTime;
};

struct clockStruct
{
	int clock;
};

void endSimulation();
int think(int endProgramSharedMemoryId);
int sendCpuRequest(int clockSharedMemoryId,
		   int endProgramSharedMemoryId);
int checkForSimulationEnd(int endProgramSharedMemoryId);

int main(int argc, char *argv[])
{
	// Open the shared memory ID and semaphore ID file for reading
        FILE *fopen(), *filePointer;
        if ((filePointer = fopen("memoryIdFile","r")) == NULL)
        {
        	// Report that the memoryIdFile file could
                // not be found, then end the program
                printf("The memoryIdFile file could not ");
                printf("be found.\n");
                return 0;
        }

	// Receive copies of the CPU simulation's semaphore IDs
	int fullSemaphoreId;
	(filePointer, "%d", &fullSemaphoreId);

	int emptySemaphoreId;
	(filePointer, "%d", &emptySemaphoreId);

	// Receive copies of the CPU simulation's shared memory IDs
	int clockSharedMemoryId;
	(filePointer, "%d", &clockSharedMemoryId);

	int readyQueueSharedMemoryId;
	fscanf(filePointer, "%d", &readyQueueSharedMemoryId);

	int endProgramSharedMemoryId;
	fscanf(filePointer, "%d", &endProgramSharedMemoryId);

	// End access to the shared memory ID and semaphore ID file
	fclose(filePointer);

	// Ensure a number was entered on the command line
	if (argc < 2)
	{
		// Request for a number to be entered on the command line
		printf("Please enter a number on the command line.\n");

		// End the program
		return 0;
	}

	// Obtain a value for N from the command line
	int N = atoi(argv[1]);

	// End the CPU simulation if the entered N value is zero; otherwise,
	// create a new job and send the job to the simulated CPU
	if (N == 0)
	{
		// Prepare the CPU simulation to end
		endSimulation();
	}
	else
	{
		// Acquire the program's process ID
		int processID = getPID();

		// Create an integer variable to track the number of CPU
		// requests sent
		int sentRequests;

		// Create an integer variable to track whether the program
		// should end
		int endProgram = 0;

		// Send N CPU requests to the simulated CPU
		for (sentRequests = 0; sentRequests < N; sentRequests++)
		{
			// Think for two to ten seconds
			endProgram = think(endProgramSharedMemoryId);

			// If the program should end, end the program properly
			if (endProgram)
			{
				// Properly prepare the program to end
				runProgramEndProcedure();
			}

			// Create and send a CPU request
			sendCpuRequest();

			// If the program should end, end the program properly
			if (endProgram)
			{
				// Properly prepare the program to end
				runProgramEndProcedure();
			}

		}
	}

	return 0;
}

void endSimulation()
{
	// Postcondition: The CPU simulation was concluded, cleaning all
	// 		  semaphores and shared memory used by the simulation

	;
}

int think(int endProgramSharedMemoryId)
{
        // Postcondition: The program printed a length of seconds for which the
        // 		  program would sleep, then slept from two to ten
        // 		  seconds; additionally, the CPU simulation was prepared
        // 		  to end if a simulation end request was found

	// Prepare a random number generator
	time_t timeVariable;
	srand((unsigned) time(&timeVariable));

	// Select a length of sleep time from two to ten seconds
	int sleepLength = (rand() % (10 - 2)) + 2;

	// Report the chosen sleep length
        printf("\t%d is thinking %d seconds\n", getpid(), guestNumber);

        // Sleep for the selected sleep length
        sleep(sleepLength);

        // Prepare the process to end if a simulation end request has been
        // delivered
        int endSimulation = checkForSimulationEnd(endProgramSharedMemoryId);

        // Return the integer of whether the process should end
        return endSimulation;
}

int sendCpuRequest(int clockSharedMemoryId,
                   int endProgramSharedMemoryId)
{
	// Postcondition: A CPU request was sent to the simulated CPU and either
	// 		  the sent CPU request completed, or the user program
	// 		  began to end the CPU simulation after detecting a
	// 		  simulation closure request

	// Prepare a random number generator
	time_t timeVariable;
	srand((unsigned) time(&timeVariable));

	// Create a struct of the job request
	struct jobInformation job;

	// Create a CPU time requirement, C, from one to five
	int C = (rand() % (5 - 1)) + 1;

	// Set the process ID of the job request
	job -> processId = getpid();

	// Set the CPU request value of the job request
	job -> cpuRequest = C;

	// Access the clock shared memory
	struct clockStruct *ram;
	ram = (int clockStruct *) shmat(clockSharedMemoryId, NULL, SHM_RND);

	// Acquire a copy of the current shared memory clock time
	int preRequestTime;
	preRequestTime = ram -> clock;

	// Set the initial clock time of the current job request
	job -> initialClockTime = preRequestTime;

	// Set the job's needed time
	job -> neededTime = C;

	// When space exists in the job queue, place this program's job into the
	// job queue
	p(empty);

	// Place the job into the job queue
	;

	// Wait for the job to end
	p(jobComplete);

	// Check to determine if the CPU simulation should end
	endSimulation = checkForSimulationEnd(endProgramSharedMemoryId);

	v(full);

	return endSimulation;
}

int checkForSimulationEnd(int endProgramSharedMemoryId)
{
	// Postcondition: If the CPU simulation should end, a 1 was returned to
	// 		  this function's caller

	// Create a value to represent whether the CPU simulation should end,
	// and initialize the value to zero
	int endSimulation = 0;

	// Record a "1" in endSimulation if the CPU simulation should end
	if ()
	{
		endSimulation = 1;
	}

	// Return the value representing whether the CPU simulation should end
	return endSimulation;
}

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

