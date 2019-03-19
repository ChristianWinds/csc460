// Christian Winds
// CSC 460
// Wednesday, March 20th, 2019
// HW9 - Daisy's Delightful Diner
// Program Description: This program runs a Dining Philosophers problem solution
// 			for at least 100 seconds.

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>

const int debug = 0;

const int clockDebug = 0;

// Set the maximum number of dinner guests
#define MAXDINNERGUESTS 5

// Set the calculation for the guest to a dinner guest's left
#define LEFT (guestNumber + MAXDINNERGUESTS - 1) % MAXDINNERGUESTS
 
// Set the calculation for the guest to a dinner guest's right
#define RIGHT (guestNumber + 1) % MAXDINNERGUESTS

// Set the semaphore value that represents a "thinking" state
#define THINKING 0

// Set the semaphore value that represents a "hungry" state
#define HUNGRY 1

// Set the semaphore value that represents a "eating" state
#define EATING 2

// Set the semaphore value that represents a "eating" state
#define DONE 3

int think(int guestNumber,
	  int sharedMemoryClockId,
	  int guestStateArraySharedMemId);
void retrieveChopsticks(int guestNumber,
			int mutexSemId,
			int dinnerGuestsSemId,
			int guestStateArraySharedMemId);
int eat(int guestNumber,
	int sharedMemoryClockId,
        int guestStateArraySharedMemId);
void test(int guestNumber,
	  int guestStateArraySharedMemId,
	  int dinnerGuestsSemId);
void placeChopsticks(int guestNumber,
		     int mutexSemId,
		     int guestStateArraySharedMemId,
		     int dinnerGuestsSemId);
int checkIfTimeIsReached(int timeMinimum, int sharedMemoryClockId);
void sleepForRandomLength(int minimumSeconds, int maximumSeconds);

int main()
{
	// Create a shared memory clock
	int sharedMemoryClockId = shmget(IPC_PRIVATE, sizeof(int), 0764);

	// Access the shared memory clock
	int* sharedMemoryClock = (int*) shmat(sharedMemoryClockId,
					      NULL,
					      SHM_RND); // * DEBUG: Possibly CORRECT

	// Set the shared memory clock to zero
	*sharedMemoryClock = 0;


	if (clockDebug)
	{
		printf("main: Attempted to set shared memory clock to zero.\nsharedMemoryClock = 0; compiles and runs.\n");
	}

	// Create an array to hold the states of the five dinner guest processes
	int dinnerGuestStateArray[5];

	// Create shared memory to hold the five dinner guests' states
	int guestStateArraySharedMemId = shmget(IPC_PRIVATE,
						sizeof(dinnerGuestStateArray),
						0764);

	// Create a mutex semaphore
	int mutexSemId = semget (IPC_PRIVATE, 1, 0777);

	// Ensure the mutex semaphore was created properly
	if (mutexSemId == -1)
	{
		// Report that the mutex semaphore creation had not
		// succeeded, then end the program
		printf("The mutex semaphore could not be created ");
		printf("correctly.\n");
		return (-1);
	}

	// Initialize the mutex semaphore
        semctl(mutexSemId, 0, SETVAL, 1);

	// Create a semaphore array holding semaphores for the five dinner
	// guests
	int dinnerGuestsSemId = semget (IPC_PRIVATE, MAXDINNERGUESTS, 0777);

	// Ensure the dinner guests semaphore was created properly
	if (dinnerGuestsSemId == -1)
	{
		// Report that the chopsticks semaphore creation had not
		// succeeded, then end the program
		printf("The dinner guests semaphore could not be created ");
		printf("correctly.\n");
		return (-1);
	}

/*	// Initialize each of the chopstick semaphores
	int semaphoreIndex;
	for (semaphoreIndex = 0; semaphoreIndex < CHOPSTICKS; semaphoreIndex++)
	{
		// Initialize the current semaphore
		semctl(chopstickSemId, semaphoreIndex, SETVAL, 1);
	}*/ // * DEBUG: May be removable

	// Create an integer variable of this process's guest number
	int guestNumber = 0;

	// Create an integer variable to hold each process's child's process ID
	int processIdOfChild = -1;

	// Create an integer variable to track the total number of existing
	// dinner guest processes
	int dinnerGuests = 0;

	// Create four additional processes
	while (dinnerGuests < MAXDINNERGUESTS)
	{
		if (debug)
		{
			printf("main: Number of dinnerGuests (before dinnerGuests++): %d\n",
			       dinnerGuests);
		}

		// Create a new process
		processIdOfChild = fork();

		dinnerGuests++;

		if (debug)
		{
			printf("main: Number of dinnerGuests (after dinnerGuests++): %d;\nprocessIDOfChild: %d\n",
			       dinnerGuests, processIdOfChild);
		}

		// If the current process is a child process, modify the
		// process's guest number and leave the process creation loop
		if (processIdOfChild == 0)
		{
			// Change the process's guest number
			guestNumber = dinnerGuests - 1;

			if (debug)
			{
				printf("main: Created guest (guestNumber) %d\n",
				       guestNumber);
			}

			break;
		}
	}

	// Begin the separate actions for the dinner guest processes and the
	// main program
	if (processIdOfChild == 0)
	{
		/*// Determine the semaphores corresponding to the dinner guests
		// to the left and right of this process's dinner guest
		int leftGuest = (guestNumber + MAXDINNERGUESTS - 1) %
				 MAXDINNERGUESTS;
		int rightGuest = (guestNumber + 1) % MAXDINNERGUESTS;*/ // * DEBUG: Disable

		// Create an integer variable to track whether the process is in
		// a "done" state; initialize the variable to 0
		int processDone = 0;

		// Enter a loop of the Dining Philosophers problem for at least
		// 100 seconds
		while (processDone == 0)
		{
			// Think for five to fifteen seconds
			processDone = think(guestNumber, 
					    sharedMemoryClockId,
					    guestStateArraySharedMemId);

			// End the process if over 100 seconds of the Dining
			// Philosophers loop have passed
			if (processDone == 1)
			{
				// Leave the Dining Philosophers loop
				break;
			}

			// Retrieve this dinner guest's chopsticks
			retrieveChopsticks(guestNumber,
					   mutexSemId,
					   dinnerGuestsSemId,
					   guestStateArraySharedMemId/*,
					   leftGuest,
					   rightGuest*/);

			// Eat for one to three seconds
			processDone = eat(guestNumber,
					  sharedMemoryClockId,
					  guestStateArraySharedMemId);

			// End the process if over 100 seconds of the Dining
			// Philosophers loop have passed
			if (processDone == 1)
			{
				// Leave the Dining Philosophers loop
				break;
			}

			// Place this process's chopsticks back
			placeChopsticks(guestNumber,
					mutexSemId,
					guestStateArraySharedMemId,
					dinnerGuestsSemId);
		}
	}
	else
	{
		// Attach to the dinner guest state shared memory
		int* guestState = (int*) shmat(guestStateArraySharedMemId,
					       NULL,
					       SHM_RND); // * DEBUG: Possibly CORRECT

		if (clockDebug)
		{
			printf("main: int* guestState = (int*) shmat(guestStateArraySharedMemId, NULL, SHM_RND); run\n");
		}

		// Create a char array to hold the names of the process states
		char *processStatesAsStrings[MAXDINNERGUESTS];

		if (clockDebug)
		{
			printf("main: char *processStatesAsStrings[MAXDINNERGUESTS]; run\n");
		}

		// Create a char string variable to hold a state's name
		char currentGuestStateString[9];

		if (clockDebug)
		{
			printf("main: char currentGuestStateString[9]; run\n");
		}

		// Create an integer variable that tracks whether all dining guest processes are not in "done" states; initialize the variable to 1
		int dinnerGuestsActive = 1;

		// Run the program until all dinner guest processes enter a
		// "done" state
		while(dinnerGuestsActive)
		{
			// Enter this process's critical section
			p(0, mutexSemId);

			if (clockDebug)
			{
				printf("main: p(0, mutexSemId); run\n");
			}


			// Prepare an integer variable to track the current
			// dinner guest process whose state is being read
			int currentDinnerGuest;

			if (clockDebug)
			{
				printf("main: int currentDinnerGuest; run\n");
			}

			// Prepare an integer variable to use to hold a dinner
			// guest's state integer
			int currentGuestStateInt;

			if (clockDebug)
			{
				printf("main: int currentGuestStateInt; run\n");
			}

			// Create char string variables for each of the four
			// dinner guest process state strings
			char guest0StateString[9];
			char guest1StateString[9];
			char guest2StateString[9];
			char guest3StateString[9];
			char guest4StateString[9];

			// Read the states of the five dinner guests
			for (currentDinnerGuest = 0;
			     currentDinnerGuest < MAXDINNERGUESTS;
			     currentDinnerGuest++)
			{
				// Read the current dinner guest's state
				currentGuestStateInt = guestState[currentDinnerGuest];

				if (clockDebug)
				{
					printf("main: currentGuestStateInt = guestState[currentDinnerGuest]; run\n");
				}

				// Store the string translation of the current
				// dinner guest's current state
				if (currentGuestStateInt == THINKING)
				{
					strcpy(currentGuestStateString, "thinking");
					if (clockDebug)
					{
						printf("main: strcpy(currentGuestStateString, \"thinking\"); run\n");
					}
				}
				else if (currentGuestStateInt == HUNGRY)
				{
					strcpy(currentGuestStateString, "hungry");
					if (clockDebug)
					{
						printf("main: strcpy(currentGuestStateString, \"hungry\"); run\n");
					}
				}
				else if (currentGuestStateInt == EATING)
				{
					strcpy(currentGuestStateString, "eating");
					if (clockDebug)
					{
						printf("main: strcpy(currentGuestStateString, \"eating\"); run\n");
					}
				}
				else if (currentGuestStateInt == DONE)
				{
					strcpy(currentGuestStateString, "done");
					if (clockDebug)
					{
						printf("main: strcpy(currentGuestStateString, \"done\"); run\n");
					}
				}
				
				// Copy the current dinner guest process's state
				// string into the respective dinner guest
				// process state string variable
				if(currentDinnerGuest == 0)
				{
					// Store the state string for dinner
					// guest 0
					strcpy(guest0StateString,
					       currentGuestStateString);
				}
				else if(currentDinnerGuest == 1)
				{
					// Store the state string for dinner
					// guest 1
					strcpy(guest1StateString,
					       currentGuestStateString);
				}
				else if(currentDinnerGuest == 2)
				{
					// Store the state string for dinner
					// guest 2
					strcpy(guest2StateString,
					       currentGuestStateString);
				}
				else if(currentDinnerGuest == 3)
				{
					// Store the state string for dinner
					// guest 3
					strcpy(guest3StateString,
					       currentGuestStateString);
				}
				else if(currentDinnerGuest == 4)
				{
					// Store the state string for dinner
					// guest 4
					strcpy(guest4StateString,
					       currentGuestStateString);
				}
			}

			// Leave this process's critical section
			v(0, mutexSemId);

			if (clockDebug)
			{
				printf("main: v(0, mutexSemId); run\n");
			}

			// Print the states of the five dinner guests
			printf("%d. %s\t %s\t %s\t %s\t %s\n",
			       *sharedMemoryClock,
			       guest0StateString,
			       guest1StateString,
			       guest2StateString,
			       guest3StateString,
			       guest4StateString);

			if (clockDebug)
                        {
                                printf("main: printf statement run\n");
			}

			// If all five dinner guest processes are in "done"
			// states, begin to end the program
			if ( (strcmp (guest0StateString, "done") == 0) &&
			     (strcmp (guest1StateString, "done") == 0) &&
			     (strcmp (guest2StateString, "done") == 0) &&
			     (strcmp (guest3StateString, "done") == 0) &&
			     (strcmp (guest4StateString, "done") == 0) )
			{
				// Flag that all dinner guest processes are
				// complete
				dinnerGuestsActive = 0;

				// Leave the clock while loop
				break;
			}

			// Sleep for one second	
			sleep (1);

			if (clockDebug)
			{
				printf("main: sleep (1); run\n");
			}

			// Increment the clock one second
			*sharedMemoryClock = *sharedMemoryClock + 1;

			if (clockDebug)
			{
				printf("main: Current clock time (After increment): %d\n", *sharedMemoryClock);
			}
		}

		// Clean the clock shared memory
		if ((shmctl(sharedMemoryClockId, IPC_RMID, NULL)) == -1)
		{
			// Report that an error occurred in cleaning the clock
			// shared memory
			printf("The shared memory clock could not be removed.\n");
		}

		// Clean the guest state array shared memory
		if ((shmctl(guestStateArraySharedMemId, IPC_RMID, NULL)) == -1)
		{
			// Report that an error occurred in cleaning the guest
			// state array shared memory
			printf("The guest state array shared memory could not be removed.\n");
		}

		// Clean the mutex semaphore
		if ((semctl(mutexSemId, 0, IPC_RMID, 0)) == -1)
		{
			// Report that an error occurred in attempting to clean
			// the mutex semaphore
			printf("%s", "The mutex semaphore could not be removed.\n");
		}

		// Clean the dinner guests semaphore
		if ((semctl(dinnerGuestsSemId, 0, IPC_RMID, 0)) == -1)
		{
			// Report that an error occurred in attempting to clean
			// the dinner guests semaphore
			printf("%s", "The dinner guests semaphore could not be removed.\n");
		}
	}

	return 0;
}

int think(int guestNumber,
	  int sharedMemoryClockId,
	  int guestStateArraySharedMemId)
{
	// Postcondition: The process that called this function slept for five
	// 		  to fifteen seconds

	if (debug)
	{
		printf("think: Guest %d is going to think. (Before sleeping).\n",
		        guestNumber);
	}

	// Sleep for five to fifteen seconds
	sleepForRandomLength(5, 15);

	if (debug)
	{
		printf("think: Guest %d has thought. (Sleep ended)\n", guestNumber);
	}

	// Prepare the process to end if over 100 seconds of the Dining
	// Philosophers loop have passed
	int processDone = checkIfTimeIsReached(100, sharedMemoryClockId); // * DEBUG: Development disable; Reenable for complete program test

/*	int processDone = 0; // * DEBUG: Temporary placeholder*/

	// If the program's minimum dining philosophers run time has been
	// reached, set this process's state to "done"
	if (processDone)
	{
		// Attach to the dinner guest state shared memory
		int* guestState = (int*) shmat(guestStateArraySharedMemId,
					       NULL,
					       SHM_RND); // * DEBUG: Possibly CORRECT

		// Set this process's state to "done"
		guestState[guestNumber] = DONE;
	}

	// Return the integer of whether the process should end
	return processDone; 
}

void retrieveChopsticks(int guestNumber,
			int mutexSemId,
			int dinnerGuestsSemId,
			int guestStateArraySharedMemId)
{
	// Postcondition: The state of the process that called this function was
	// 		  set to "hungry"

	if (debug)
	{
		printf("guestNumber %d: retrieveChopsticks: begun\n", guestNumber);
	}

	// Begin this function's critical section
	p(0, mutexSemId);
	
	if (debug)
	{
		printf("guestNumber %d: retrieveChopsticks: p(0, mutexSemId); run\n", guestNumber);
	}

	// Create an array to permit access to the dinner guest state array in shared memory
	int *dinnerGuestStateArray[5];

	if (debug)
	{
		printf("guestNumber %d: retrieveChopsticks: int *dinnerGuestStateArray[5]; run\n", guestNumber);
	}

	// Attach to the dinner guest state shared memory
	/*char*//*int *ram;*/ // * DEBUG: Temporary disable; may be incorrect
//	ram = (/*char*/int )/* * */dinnerGuestStateArray[5] shmat (guestStateArraySharedMemId,
//						      NULL,
//						      SHM_RND); // * DEBUG: Temporary disable
	int* guestState = (int*) shmat(guestStateArraySharedMemId, NULL, SHM_RND); // * DEBUG: Possibly CORRECT

	if (debug)
	{
		printf("guestNumber %d: retrieveChopsticks: int* guestState = (int*) shmat(guestStateArraySharedMemId, NULL, SHM_RND); run\n", guestNumber);
	}

	// Update this process's state to "hungry"
	/*strcpy (ram[guestNumber], "hungry");*/ // * DEBUG: Development disable; May be needed if strcpy is necessary
	guestState[guestNumber] = HUNGRY;

	if (debug)
	{
		printf("guestNumber %d: retrieveChopsticks: ram[guestNumber] = HUNGRY; run\n", guestNumber);
	}

	// Attempt to acquire this process's two chopsticks
	test(guestNumber, guestStateArraySharedMemId, dinnerGuestsSemId);

	if (debug)
	{
		printf("guestNumber %d: retrieveChopsticks: test(guestNumber, guestStateArraySharedMemId, dinnerGuestsSemId); run\n", guestNumber);
	}

	// Leave the critical section
	v(0, mutexSemId);

	if (debug)
	{
		printf("guestNumber %d: retrieveChopsticks: v(0, mutexSemId); run\n", guestNumber);
	}

	// If this guest's chopsticks were not obtained, block the guest's process
	p(guestNumber, dinnerGuestsSemId);

	if (debug)
	{
		printf("guestNumber %d: retrieveChopsticks: p(guestNumber, dinnerGuestsSemId); run\nguestNumber %d: retrieveChopsticks: colcluded; ending function...\n", guestNumber, guestNumber);
	}
}

int eat(int guestNumber,
	int sharedMemoryClockId,
	int guestStateArraySharedMemId)
{
	// Postcondition: The process that called this function slept for one to
	// 		  three seconds, then returned a 1 if the dining
	// 		  philosophers loop ran for at least 100 seconds. A 0
	// 		  was returned if the dining philosophers loop has not
	// 		  yet run for 100 seconds.

	if (debug)
	{
		printf("eat: Guest %d will eat. (Before sleeping)\n", guestNumber);
	}

	// Sleep for one to three seconds
	sleepForRandomLength(1, 3);

	if (debug)
	{
		printf("think: Guest %d has eaten. (Sleep ended)\n", guestNumber);
	}

	// Prepare the process to end if over 100 seconds of the Dining
	// Philosophers loop have passed
	int processDone = checkIfTimeIsReached(100, sharedMemoryClockId); // * DEBIG: Temporary disable; reenable for full program test

	/*int processDone = 0; // * DEBUG: Temporary placeholder code*/

	// If the program's minimum dining philosophers run time has been
	// reached, set this process's state to "done"
	if (processDone)
	{
		// Attach to the dinner guest state shared memory
		int* guestState = (int*) shmat(guestStateArraySharedMemId, NULL, SHM_RND); // * DEBUG: Possibly CORRECT

		// Set this process's state to "done"
		guestState[guestNumber] = DONE;
	}

	return processDone;
}

void test(int guestNumber, int guestStateArraySharedMemId, int dinnerGuestsSemId)
{
	// Postcondition: The dinner guest process that reached this function
	// 		  entered an "eating" state if the processes left and
	// 		  right of the arrived dinner guest are not in "eating"
	// 		  states

/*	// Create a dinner guest state char array to permit access to
	// the dinner guest state shard memory
	char *dinnerGuestStateArray[5];

	// Attach to the dinner guest state shared memory
	char *ram;
	ram = (char *dinnerGuestStateArray[5]) shmat (guestStateArraySharedMemId,
						      NULL,
						      SHM_RND);

	// Retrieve the dinner guest state strings required for the chopstick
	// presence check
	char *leftGuestState;
	strcpy(leftGuestState, ram[LEFT]);

	char *rightGuestState;
	strcpy(rightGuestState, ram[RIGHT]);

	char *currentGuestState;
	strcpy(currentGuestState, ram[guestNumber]); */ // * DEBUG: Development disable; may be unneeded

	// Update the arrived dinner guest process's state to "eating" if the
	// processes to the left and right of the arrived process are not in
	// "eating" states
/*	if ((strcmp (currentGuestState, "hungry") == 0) &&
	    ((strcmp(leftGuestState, "eating") != 0) && (strcmp(rightGuestState, "eating") != 0) )) */ // * DEBUG: Development disable; may be unneeded

	// Create a dinner guest integer array to permit access to
	// the dinner guest state shard memory
	int *dinnerGuestStateArray[5];

	// Attach to the dinner guest state shared memory
	int *ram;
	/*ram = (int *dinnerGuestStateArray[5]) shmat (guestStateArraySharedMemId,
						     NULL,
						     SHM_RND);*/ // * DEBUG: Disabled; may be incorrect
	int* guestState = (int*) shmat(guestStateArraySharedMemId, NULL, SHM_RND); // * DEBUG: Possibly CORRECT

	// Update the arrived dinner guest process's state to "eating" if the
	// processes to the left and right of the arrived process are not in
	// "eating" states
/*	if (ram[guestNumber] == HUNGRY) &&
	   ((ram[LEFT] != EATING) && (ram[RIGHT] != EATING))*/ // * DEBUG: May be incorrect
	if ((guestState[guestNumber] == HUNGRY) &&
	   ((guestState[LEFT] != EATING) && (guestState[RIGHT] != EATING)))
	{
		// Update this process's state to "eating"
		/*strcpy (ram[guestNumber], "eating"); */ // * DEBUG: Development disable; may be unneeded unless string copy is required
		/*ram[guestNumber] = EATING;*/ // * DEBUG: May be incorrect
		guestState[guestNumber] = EATING;

		// Awake a blocked dinner guest process if any dinner guest
		// processes are blocked
		v(guestNumber, dinnerGuestsSemId);
	}
}

void placeChopsticks(int guestNumber,
		     int mutexSemId,
		     int guestStateArraySharedMemId,
		     int dinnerGuestsSemId)
{
	// Postcondition: The process that arrived at this function was placed
	// 		  into a "thinking" state,

	// Begin this function's critical section
	p(0, mutexSemId);

	// Create an array to permit access to the dinner guest state array in shared memory
	char *dinnerGuestStateArray[5];

	// Attach to the dinner guest state shared memory
/*	char *ram;*/ // * DEBUG: Disabled; may be incorrect
/*	ram = (char *dinnerGuestStateArray[5]) shmat (guestStateArraySharedMemId,
						      NULL,
						      SHM_RND); */ // * DEBUG: Disable; May be incorrect
	int* guestState = (int*) shmat(guestStateArraySharedMemId, NULL, SHM_RND); // * DEBUG: Possibly CORRECT

	// Update this process's state to "thinking"
/*	strcpy (ram[guestNumber], "thinking");*/ // * DEBUG: Disabled; may be incorrect
	guestState[guestNumber] = THINKING;

	// Determine if the left dinner guest may begin eating
	test(LEFT, guestStateArraySharedMemId, dinnerGuestsSemId);

	// Determine if the right dinner guest may begin eating
	test(RIGHT, guestStateArraySharedMemId, dinnerGuestsSemId);

	// Leave this function's critical section
	v(0, mutexSemId);
}

int checkIfTimeIsReached(int timeMinimum, int sharedMemoryClockId)
{
	// Postcondition: If the shared memory clock's time is the received
	// 		  minimum time or more when this function checks the
	// 		  clock, a 1 was returned to this function's caller

	// Create an integer variable to represent whether the minimum time has
	// been reached
	int timeReached;

	// Access the shared memory clock
	int* sharedMemoryClock = (int*) shmat(sharedMemoryClockId, NULL, SHM_RND); // * DEBUG: Possibly CORRECT

	if (*sharedMemoryClock >= timeMinimum)
	{
		// Flag the clock's arrival to the minimum time or more
		timeReached = 1;
	}
	else
	{
		// Flag that the clock has not yet reached the minumum time
		timeReached = 0;
	}

	// Return the integer representing whether the minimum time was reached
	return timeReached;
}
void sleepForRandomLength(int minimumSeconds, int maximumSeconds)
{
	// Postcondition: The process that called this function slept for a
	// 		  random number of seconds between the received minimum
	// 		  and maximum numbers of seconds

	// Prepare the random number generator
	time_t timeVariable;
	srand((unsigned) time(&timeVariable));

	// Create a length of time to sleep
	int sleepLength = (rand() % (maximumSeconds - minimumSeconds))
			   + minimumSeconds;

	// Sleep for the selected sleep length
	sleep(sleepLength);
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
