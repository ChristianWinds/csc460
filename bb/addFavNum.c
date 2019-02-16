// Christian Winds
// CSC460
// Monday, February 18th, 2019
// HW6 - addFavNum
// Program Description: This program places a number entered by the program's
// user into a shared bulletin board in the favorite number field location of
// this program's author's bulletin board record.

#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>

struct bulletinBoardRecord
{
	int id;
	char name[20];
	int favoriteNumber;
	char favoriteFood[30];
};

int main(int argc, char *argv[])
{
	// Ensure a number was entered on the command line
	if (argc < 2)
	{
		printf("Enter an integer when starting the program.\n");
		return 0;
	}

	// Store the number entered on the command line
	int enteredNumber = atoi(argv[1]);

	// Open the file holding the shared memory of the bulletin board
	FILE *fopen(), *fp;
	if ((fp = fopen("/tmp/allen/csc460/bb/BBID.txt","r")) == NULL)
	{
		// Print an error message to indicate the shared memory file
		// could not be opened, then end the program
		printf("The shared memory ID file could not be opened.\n");
		return 0;
	}
	
	// Create an integer variable to hold the bulletin board's shared memory
	// ID
	int sharedMemoryId;

	// Retrieve the shared memory ID of the bulletin board
	fscanf(fp, "%d", &sharedMemoryId);
	fclose(fp);

	// Create a struct pointer to use to access the bulletin board shared
	// memory
	struct bulletinBoardRecord *bulletinBoard;

	// Attach to the bulletin board shared memory
	bulletinBoard = (struct bulletinBoardRecord *) shmat(sharedMemoryId, NULL, SHM_RND);

	// Set the bulletin board ID of the record to change
	const int BULLETINBOARDID = 23;

	// Write the entered integer to the program author's favorite number
	// field on the bulletin board
	bulletinBoard += BULLETINBOARDID - 1;
	bulletinBoard -> favoriteNumber = enteredNumber;

	return 0;
}
