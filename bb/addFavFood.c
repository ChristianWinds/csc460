// Christian Winds
// CSC460
// Monday, February 18th, 2019
// HW6 - addFavFood
// Program Description: This program places a favorite food entered by the
// program's user into a shared bulletin board in the favorite food field
// location of this program's author's bulletin board record.

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
	// Ensure a food name was entered on the command line
	if (argc < 2)
	{
		// Print an error message if no food name was entered on the
		// command line, then end the program
		printf("Enter a food name when starting the program.\n");
		return 0;
	}

	// Ensure the food name entered is no longer than 29 characters
	if (strlen(argv[1]) > 29)
	{
		// Print an error message if the food name entered is longer
		// than 29 characters, then end the program
		printf("Enter a food name with 29 characters or less (including spaces and punctuation).\n");
		return 0;
	}

	// Create a variable to hold the entered food name
	char enteredFood[30];

	// Store the food name received from the command line
	strcpy(enteredFood, argv[1]);

	// Open the file holding the shared memory ID of the bulletin board
	FILE *fopen(), *fp;
	if ((fp = fopen("/tmp/allen/csc460/bb/BBID.txt","r")) == NULL)
	{
		// Print an error message to indicate the shared memory file could
		// not be opened, then end the program
		printf("The shared memory ID file could not be opened.\n");
		return 0;
	}
	
	// Retrieve the shared memory ID of the bulletin board
	int sharedMemoryId;
	fscanf(fp, "%d", &sharedMemoryId);
	fclose(fp);

	// Create a bulletin board struct to allow access to the shared memory
	// bulletin board
	struct bulletinBoardRecord *bulletinBoard;

	// Attach to the bulletin board shared memory
	bulletinBoard = (struct bulletinBoardRecord *) shmat(sharedMemoryId, NULL, SHM_RND);

	// Set the bulletin board ID of the record to change
	const int BULLETINBOARDID = 23;

	// Write the entered food to the program author's favorite food
	// field on the bulletin board
	bulletinBoard += BULLETINBOARDID - 1;
	strcpy(bulletinBoard -> favoriteFood, enteredFood);

	return 0;
}
