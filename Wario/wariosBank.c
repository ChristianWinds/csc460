// Christian Winds
// CSC 460
// Monday, February 25th, 2019
// HW7 - Wario's Bank
// Program Description: This program creates a bank account with a balance of
// 			1000, as well as sixteen deposit processes and sixteen
// 			withdrawal processes that respectively deposit and
// 			withdraw amounts from the balance. After all deposits
// 			and withdrawals are complete, the balance should again
// 			be 1000 from the combination of balance transactions.

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <math.h>

int debugMode = 1;

struct bankBalance
{
	int balance;
};

struct processInformation
{
	char processTask[9];
	int value;
	int transactionAmount;
};

int bankExistenceCheck();
int integerStringCheck(char charString[]);
void printArgumentEntryError();
void printNoBankFilesError();

int main (int argc, char *argv[])
{
	// Ensure that a correct number of arguments may be present upon the
	// command line
	if (argc > 2)
	{
		// Report that an invalid argument was entered, then end the
		// program
		printArgumentEntryError();
		return 0;
	}

	// Select the program mode requested by the command line arguments
	if (argc == 1)
	{
		// Run the "no arguments" program mode
		if (debugMode)
		{
			printf("* \"No Arguments\" mode entered.\n");
		}

		if (debugMode)
		{
			printf("* Attempting to check bank file existence.\n");
		}
		// Check to determine if bank files already exist
		if (bankExistenceCheck() == 1)
		{
			// Report that bank files already exist, then end the
			// program
			printf("A bank is already open.\n");
			return 0;
		}
		if (debugMode)
		{
			printf("* Bank file existence checked.\n");
		}

		// Create a binary semaphore to manage process access to the bank
		// balance, and store the binary semaphore's ID
		int mutexSemaphoreId = semget(IPC_PRIVATE, 1, 0777);

		// Ensure the mutex semaphore was created
		if (mutexSemaphoreId == -1)
		{
			// Report that an error occurred when attempting to create
			// the mutex semaphore, then end the program
			printf("* An error occurred when attempting to use ");
			printf("semget to create a mutex semaphore.\n");
			return (-1);
		}

		// Initialize the semaphore
		semctl(mutexSemaphoreId, 0, SETVAL, 1);

		// Create a file to hold the semaphore ID
		FILE *fopen(), *filePointer;
	        if ((filePointer = fopen("bankIDs","w")) == NULL)
	        {
			// Report that the file bankIDs could not be found, then
			// end the program
	                printf("The file bankIDs could not be found.\n");
	                return 0;
	        }

		// Store the ID of the binary semaphore into the external ID
		// file
		fprintf(filePointer, "%d\n", mutexSemaphoreId);

		// Create bank balance shared memory
		int balanceSharedMemId;
		balanceSharedMemId = shmget(IPC_PRIVATE, sizeof(int), 0777);

		// Ensure the balance shared memory was created
		if (balanceSharedMemId == -1)
		{
			// Report that the shared memory could not be created,
			// then end the program
			printf("The bank balance shared memory could not be ");
			printf("created.\n");
			return 0;
		}

		// Store the ID of the bank balance shared memory in the bankIDs
		// file
		fprintf(filePointer, "%d", balanceSharedMemId);

		// Set the balance shared memory's balance
		struct bankBalance *ram;
		ram = (struct bankBalance *) shmat (balanceSharedMemId, NULL,
						    SHM_RND);
		ram -> balance = 1000;

		// Close the file pointer to the bankIDs file
		fclose(filePointer);
	}
	else
	{
		// Check whether the first argument on the command line is an
		// integer
		if (integerStringCheck(argv[1]) == 1)
		{
			// Run the "integer argument" program mode
			if (debugMode)
			{
				printf("* Integer argument mode entered.\n");
			}

			// Ensure that bank files exist
			if (bankExistenceCheck() == 0)
			{
				// Report that no bank files exist, then end the
				// program
				printNoBankFilesError();
				return 0;
			}

			// Create an integer variable for the number of times to
			// run the simulation
			int N = atoi(argv[1]);

			// Ensure that the number of times to run the simulation
			// is between 1 and 100; end the program if a number out
			// of range was entered
			if ((N < 1) || (N > 100))
			{
				// Print an error message requesting entry of an
				// integer between 1 and 100
				printf("Please enter an integer from 1 to 100.\n");

				return 0;
			}

			// Open the bankIDs file for reading
			FILE *fopen(), *filePointer;
		        if ((filePointer = fopen("bankIDs","r")) == NULL)
		        {
				// Report that the bankIDs file could not be
				// found, then end the program
		                printf("The bankIDs file could not be found.\n");
		                return 0;
		        }

			// Store the mutex semaphore ID
			int mutexSemaphoreId;
			fscanf(filePointer, "%d", &mutexSemaphoreId);

			// Store the balance shared memory ID
			int balanceSharedMemId;
			fscanf(filePointer, "%d", &balanceSharedMemId);

			// Close the file pointer to the bankIDs file
			fclose(filePointer);

			// Access the balance shared memory
			struct bankBalance *ram;
			ram = (struct bankBalance *) shmat(balanceSharedMemId,
							   NULL, SHM_RND);

			// Create a struct to hold process information
			struct processInformation *processValues;

			// Set the process's processTask to "deposit"
			strcpy(processValues -> processTask, "deposit");

			// Set this process's processInformation values
			processValues -> value = 0;
			processValues -> transactionAmount = pow(2.0,
								 processValues -> value);

			// Create an integer variable to hold each the process ID
			// of each process's child
			int processIdOfChild = -1;

			// Have a total of sixteen processes with values from
			// zero to fifteen
			int depositProcesses = 1;
			const int MAXDEPOSITPROCESSES = 16;
			while ((depositProcesses < MAXDEPOSITPROCESSES) &&
			       (processIdOfChild != 0))
			{
				// Create a new deposit process
				processIdOfChild = fork();

				// Change the new process's processInformation
				// values
				if (processIdOfChild == 0)
				{
					// Assign this process's value
					processValues -> value = depositProcesses;

					// Assign this process's amount to
					// deposit to the bank balance
					processValues -> transactionAmount = pow(2.0,
										 processValues -> value);
				}
			
				depositProcesses++;
			}

			// Create a child process from each of the sixteen
			// deposit processes
			processIdOfChild = fork();

			// Change the processInformation content of the child
			// process
			if (processIdOfChild == 0)
			{
				strcpy(processValues -> processTask, "withdraw");
			}

			// Perform the necessary balance transactions N times,
			// depositing into the bank account if a parent process,
			// or withdrawing from the bank account if a child
			// process
			int simulationsRun = 0;
			for (simulationsRun; simulationsRun < N; simulationsRun++)
			{
				// If the mutex semaphore permits balance value
				// access, perform the necessary balance
				// transaction
				p(0, mutexSemaphoreId);

				// Determine the process task to perform
				if (strcmp(processValues -> processTask,
					   "deposit") == 0)
				{
					// Deposit the process's value into the
					// balance
					ram -> balance += processValues -> transactionAmount;
				}
				else if (strcmp(processValues -> processTask,
					        "withdraw") == 0)
				{
					// Withdraw the process's value from the
					// balance
					ram -> balance -= processValues -> transactionAmount;
				}

				// Permit another process to access the balance
				// shared memory
				v(0, mutexSemaphoreId);
			}
		}
		else
		{
			// Select the program mode specified by the argument
			// char string
			if (strcmp(argv[1], "balance") == 0)
			{
				// Run the "balance" argument program mode
				if (debugMode)
				{
					printf("* \"balance\" argument mode entered.\n");
				}

				// Check to determine if bank files exist
				if (bankExistenceCheck() == 0)
				{
					// Report that no bank files exist, then
					// end the program
					printNoBankFilesError();
					return 0;
				}

				// Open the bankIDs file for reading
				FILE *fopen(), *filePointer;
		                if ((filePointer = fopen("bankIDs","r")) == NULL)
		                {
					// Report that the bankIDs file could
					// not be found, then end the program
		                        printf("The bankIDs file could not ");
					printf("be found.\n");
		                        return 0;
		                }

				// Create buffer variables to skip to the
				// balance shared memory ID in the bankIDs file
				int mutexSemaphoreIdBuffer;

				// Read the mutex semaphore ID from the bankIDs
				// file
				fscanf(filePointer, "%d",
				       &mutexSemaphoreIdBuffer);
					
				// Retrieve the balance shared memory ID from
				// the bankIDs file
				int balanceSharedMemId;
				fscanf(filePointer, "%d", &balanceSharedMemId);

				if (debugMode)
				{
					printf("* Value read as balance shared memory ID: %d\n", balanceSharedMemId);
				}

				// End access to the bankIDs file
				fclose(filePointer);

				// Access the balance shared memory
				struct bankBalance *ram;
				ram = (struct bankBalance *) shmat(balanceSharedMemId,
								   NULL, SHM_RND);

				// Read the balance in the balance shared memory
				int currentBalance;
				currentBalance = ram -> balance;

				// Print the balance from the balance shared
				// memory
				printf("Current bank balance: %d\n",
				       currentBalance);
			}
			else if (strcmp(argv[1], "die") == 0)
			{
				// Run the "die" argument program mode
				if (debugMode)
				{
					printf("* \"die\" argument mode entered.\n");
				}

				// Check to ensure bank files already exist
				if (bankExistenceCheck() == 0)
				{
					// Report that no bank files exist, then
					// end the program
					printNoBankFilesError();
					return 0;
				}

				// Open the bankIDs file for reading
				FILE *fopen(), *filePointer;
		                if ((filePointer = fopen("bankIDs","r")) == NULL)
		                {
					// Report that the bankIDs file could
					// not be found, then end the program
		                        printf("The bankIDs file could not be ");
					printf("found.\n");
		                        return 0;
		                }

				// Retrieve the active mutex semaphore's ID from
				// the bankIDs file
				int mutexSemaphoreId;
				fscanf(filePointer, "%d", &mutexSemaphoreId);

				// Retrieve the active balance shared memory's
				// ID from the bankIDs file
				int balanceSharedMemId;
				fscanf(filePointer, "%d", &balanceSharedMemId);

				// End access to the bankIDs file
				fclose(filePointer);

				// Remove the active mutex semaphore
				if ((semctl(mutexSemaphoreId, 0, IPC_RMID, 0)) == -1)
				{
					// Report that the mutex semaphore could
					// not be removed.
					printf("The mutex semaphore could not ");
					printf("be properly removed.\n");
				}

				// Remove the active balance shared memory
                        	if ((shmctl(balanceSharedMemId, IPC_RMID, NULL)) == -1)
	                        {
	                                // Report that the balance shared memory
	                                // could not be removed
	                                printf("The balance shared memory ");
					printf("could not be properly ");
					printf("removed.\n");
	                        }
			}
			else
			{
				// Report that an incorrect argument was entered
				printArgumentEntryError();
			}
		}
	}

	return 0;
}

int bankExistenceCheck()
{
	// Potcondition: If a bank already exists for the program, a 1 was
	// 		 returned to this function's caller. If a bank does not
	// 		 already exist, a 0 was returned to this function's
	// 		 caller.

	if (debugMode)
	{
		printf("bankExistenceCheck entered.\n");
	}

	// Create an integer variable as a flag for whether bank files exist;
	// initialize the variable to 1
	int bankExists = 1;

	// Look for a "bankIDs" file in the current directory to determine if
	// bank files already exist
	if (debugMode)
	{
		printf("Attempting to open bankIDs file.\n");
	}

	FILE *fopen(), *filePointer;
        if ((filePointer = fopen("bankIDs","r")) == NULL)
        {
		// Flag that bank files do not exist
		bankExists = 0;
        }

	if (debugMode)
	{
		printf("bankIDs file opened.\n");
	}

	if (debugMode)
	{
		printf("Attempting to close bankIDs file pointer. (DISABLED)\n");
	}

	// Close the pointer to the bankIDs file
	/*fclose(filePointer);*/ // * DEBUG: Temporarily disabled

	if (debugMode)
	{
		printf("bankIDs file pointer closed. (DISABLED)\n");
	}

	if (debugMode)
	{
		printf("Leaving bankExistenceCheck function.\n");
	}

	return bankExists;
}

int integerStringCheck(char charString[])
{
	// Postcondition: An integer for whether the received char string held
	// 		  only an integer was returned to the function's caller.
	// 		  If an integer was held, a 1 was returned to this
	// 		  function's caller. If content other than an integer
	// 		  was found, a 0 was returned to this function's caller.

	// Create an integer variable to track whether the received string is an
	// integer
	int integerPresent = 1;

	// Create an integer to track the char string index accessed
	int stringIndex = 0;

	// If the first character of the received string is a negative sign,
	// skip digit checking of the first string character and check if the
	// string may still hold an integer
	if (strncmp(charString, "-", 1) == 0)
	{
		if (debugMode)
		{
			printf("Negative sign found at start of argument string.\n");
		}

		// Skip digit checking of the first string character
		stringIndex++;

		// Check whether the string may still hold an integer after the
		// negative sign
		if (!(stringIndex < strlen(charString)))
		{
			if (debugMode)
			{
				printf("Insufficient room found for integer after argument string negative sign.\n");
			}

			// Flag that the charString does not have room for an
			// integer after the negative sign
			integerPresent = 0;
		}
	}

	// Check whether each unread character of the received string is a digit
	while ((stringIndex < strlen(charString)) &&
	       (integerPresent == 1))
	{
		// Flag the presence of a non-digit character if a string
		// character is not a digit
		if (!(isdigit(charString[stringIndex])))
			integerPresent = 0;

		// Access the next index in the char string
		stringIndex++;
	}

	return integerPresent;
}

void printArgumentEntryError()
{
	// Postcondition: An error message for an incorrect argument entry was
	// 		  printed to the screen.

	// Report that an incorrect argument was entered
	printf("An invalid argument was entered.\n");
}

void printNoBankFilesError()
{
	// Postcondition: An error message for no bank files existing was
	// 		  printed to the screen.

	// Report that no bank files exist
	printf("No bank files exist.\n");
}
