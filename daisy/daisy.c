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

// Set the semaphore value that represents a "done" state
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
	int *sharedMemoryClock = (int*) shmat(sharedMemoryClockId,
					      NULL,
					      SHM_RND);

	// Set the shared memory clock to zero
	*sharedMemoryClock = 0;

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
		// Report that the dinner guests semaphore creation had not
		// succeeded, then end the program
		printf("The dinner guests semaphore could not be created ");
		printf("correctly.\n");
		return (-1);
	}

	// Create an integer variable to hold the dinner guest process guest
	// number
	int guestNumber = 0;

	// Create an integer variable to hold each process's child's process ID
	int processIdOfChild = -1;

	// Create an integer variable to track the total number of existing
	// dinner guest processes
	int dinnerGuests = 0;

	// Create five dinner guest processes
	while (dinnerGuests < MAXDINNERGUESTS)
	{
		// Create a new dinner guest process
		processIdOfChild = fork();

		dinnerGuests++;

		// If the current process is a child process, modify the
		// process's guest number and leave the process creation loop
		if (processIdOfChild == 0)
		{
			// Change the process's guest number
			guestNumber = dinnerGuests - 1;

			// Leave the dinner guest process creation loop
			break;
		}
	}

	// Begin the separate actions for the dinner guest processes and the
	// main program
	if (processIdOfChild == 0)
	{
		// Create an integer variable to track whether the dinner guest
		// process is in a "done" state; initialize the variable to 0
		int processDone = 0;

		// Enter a loop of the Dining Philosophers problem for at least
		// 100 seconds
		while (processDone == 0)
		{
			// Think for five to fifteen seconds, and check whether
			// at least 100 seconds of the Dining Philosophers loop
			// have run
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
					   guestStateArraySharedMemId);

			// Eat for one to three seconds, and check whether at
			// least 100 seconds of the Dining Philosophers loop
			// have passed
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
		int *guestState = (int*) shmat(guestStateArraySharedMemId,
					       NULL,
					       SHM_RND);

		// Create a char string variable to hold a state's name
		char currentGuestStateString[9];

		// Create an integer variable that tracks whether all dining
		// guest processes are not in "done" states; initialize the
		// variable to 1
		int dinnerGuestsActive = 1;

		// Run the program until all dinner guest processes enter a
		// "done" state
		while(dinnerGuestsActive)
		{
			// Enter this process's critical section
			p(0, mutexSemId);

			// Prepare an integer variable to track the current
			// dinner guest process whose state is being read
			int currentDinnerGuest;

			// Prepare an integer variable to use to hold a dinner
			// guest's state integer
			int currentGuestStateInt;

			// Create char string variables for the states of each
			// of the five dinner guest processes
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

				// Store the string translation of the current
				// dinner guest's current state
				if (currentGuestStateInt == THINKING)
				{
					strcpy(currentGuestStateString, "thinking");
				}
				else if (currentGuestStateInt == HUNGRY)
				{
					strcpy(currentGuestStateString, "hungry  ");
				}
				else if (currentGuestStateInt == EATING)
				{
					strcpy(currentGuestStateString, "eating  ");
				}
				else if (currentGuestStateInt == DONE)
				{
					strcpy(currentGuestStateString, "done    ");
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

			// Print the states of the five dinner guests
			printf("%d. %s\t %s\t %s\t %s\t %s\n",
			       *sharedMemoryClock,
			       guest0StateString,
			       guest1StateString,
			       guest2StateString,
			       guest3StateString,
			       guest4StateString);

			// If all five dinner guest processes are in "done"
			// states, begin to end the program
			if ( (strcmp (guest0StateString, "done    ") == 0) &&
			     (strcmp (guest1StateString, "done    ") == 0) &&
			     (strcmp (guest2StateString, "done    ") == 0) &&
			     (strcmp (guest3StateString, "done    ") == 0) &&
			     (strcmp (guest4StateString, "done    ") == 0) )
			{
				// Flag that all dinner guest processes are
				// complete
				dinnerGuestsActive = 0;

				// Leave the clock while loop
				break;
			}

			// Sleep for one second	
			sleep (1);

			// Increment the clock by one second
			*sharedMemoryClock = *sharedMemoryClock + 1;
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
	// 		  to fifteen seconds, and if at least 100 seconds of the
	// 		  Dining Philosophers loop had run, a 1 was returned to
	// 		  this process's caller. If 100 seconds of the Dining
	// 		  Philosophers loop had not yet run, a 0 was returned to
	// 		  this function's caller instead.

	// Sleep for five to fifteen seconds
	sleepForRandomLength(5, 15);

	// Prepare the process to end if over 100 seconds of the Dining
	// Philosophers loop have passed
	int processDone = checkIfTimeIsReached(100, sharedMemoryClockId);

	// If the program's minimum dining philosophers run time has been
	// reached, set this process's state to "done"
	if (processDone)
	{
		// Attach to the dinner guest state shared memory
		int *guestState = (int*) shmat(guestStateArraySharedMemId,
					       NULL,
					       SHM_RND);

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
	// Postcondition: The state of the dining guest process that called this
	// 		  function was set to "hungry", and this function's
	// 		  caller received two chopsticks

	// Begin this function's critical section
	p(0, mutexSemId);
	
	// Create an array to permit access to the dinner guest state array in
	// shared memory
	int *dinnerGuestStateArray[5];

	// Attach to the dinner guest state shared memory
	int *guestState = (int*) shmat(guestStateArraySharedMemId, NULL, SHM_RND);

	// Update this process's state to "hungry"
	guestState[guestNumber] = HUNGRY;

	// Attempt to acquire this process's two chopsticks
	test(guestNumber, guestStateArraySharedMemId, dinnerGuestsSemId);

	// Leave the critical section
	v(0, mutexSemId);

	// If this guest's chopsticks were not obtained, block the guest's process
	p(guestNumber, dinnerGuestsSemId);
}

int eat(int guestNumber,
	int sharedMemoryClockId,
	int guestStateArraySharedMemId)
{
	// Precondition: The dining guest process that called this function held
	// 		 that process's two chopsticks
	// Postcondition: The process that called this function slept for one to
	// 		  three seconds, then this function returned a 1 if the
	// 		  dining philosophers loop ran for at least 100 seconds.
	// 		  A 0 was instead returned by this function if the
	// 		  dining philosophers loop had not yet run for 100
	// 		  seconds.

	// Sleep for one to three seconds
	sleepForRandomLength(1, 3);

	// Prepare the process to end if over 100 seconds of the dining
	// philosophers loop have passed
	int processDone = checkIfTimeIsReached(100, sharedMemoryClockId);

	// If the program's minimum dining philosophers run time has been
	// reached, set the current process's state to "done"
	if (processDone)
	{
		// Attach to the dinner guest state shared memory
		int *guestState = (int*) shmat(guestStateArraySharedMemId, NULL, SHM_RND);

		// Set this process's state to "done"
		guestState[guestNumber] = DONE;
	}

	// Return the integer representing whether the current process is in a
	// "done" state
	return processDone;
}

void test(int guestNumber, int guestStateArraySharedMemId, int dinnerGuestsSemId)
{
	// Postcondition: The dinner guest process that reached this function
	// 		  entered an "eating" state if the processes left and
	// 		  right of the arrived dinner guest were not in "eating"
	// 		  states

	// Create a dinner guest integer array to permit access to
	// the dinner guest state shared memory
	int *dinnerGuestStateArray[5];

	// Attach to the dinner guest state shared memory
	int *guestState = (int*) shmat(guestStateArraySharedMemId, NULL, SHM_RND);

	// Update the arrived dinner guest process's state to "eating" if the
	// processes to the left and right of the arrived process are not in
	// "eating" states
	if ((guestState[guestNumber] == HUNGRY) &&
	   ((guestState[LEFT] != EATING) && (guestState[RIGHT] != EATING)))
	{
		// Update this process's state to "eating"
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
	// Postcondition: The dinner guest process that arrived at this function
	// 		  was placed into a "thinking" state, and at least one
	// 		  of the dinner guest processes at this function's
	// 		  caller process's left and right was placed into an
	// 		  "eating" state if a pair of chopsticks was available
	// 		  for the process

	// Begin this function's critical section
	p(0, mutexSemId);

	// Create an array to permit access to the dinner guest state array in shared memory
	char *dinnerGuestStateArray[5];

	// Attach to the dinner guest state shared memory
	int *guestState = (int*) shmat(guestStateArraySharedMemId, NULL, SHM_RND);

	// Update the current process's state to "thinking"
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
	// Postcondition: If the shared memory clock's time was the received
	// 		  minimum time or more when this function checked the
	// 		  clock, a 1 was returned to this function's caller

	// Create an integer variable to represent whether the minimum time has
	// been reached
	int timeReached;

	// Access the shared memory clock
	int *sharedMemoryClock = (int*) shmat(sharedMemoryClockId,
					      NULL,
					      SHM_RND);

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
