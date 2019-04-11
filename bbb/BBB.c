// Christian Winds
// CSC 460
// Friday, April 12th, 2019
// HW10 - Big Bad Bowser's Dispatcher
// Program Description: This program simulates a CPU dispatcher by creating a
// 			simlated CPU and simulated dispatcher to replicate
// 			movement of jobs into a CPU and out of a CPU.

#define RUNCLOCK 0
#define RUNCPU 1
#define RUNDISPATCHERA 2
#define RUNDISPATCHERB 3

struct userInformation
{
	int processID;
	int neededTime;
};

int main(int argc, char *argv[])
{
	// Create an integer variable to hold a process number for each process
	int processNumber = 0;

	// Initialize the CPU simulation system
	initializeSystem();

	// Run each process's task until the simulation is instructed to end
	if (processNumber == RUNCLOCK)
	{
		// Run the simulation's clock until a program end instruction is
		// detcted
		runClock();
	}
	else if (processNumber == RUNCPU)
	{
		// Create and run a simulated CPU to run jobs
		runCPU();
	}
	else if (processNumber == RUNDISPATCHERA)
	{
		// Run a dispatcher function to place jobs into the simulated CPU
		runDispatcherA();
	}
	else if (processNumber == RUNDISPATCHERB)
	{
		// Run a dispatcher function to remove jobs from the CPU and
		// move the jobs to required locations
		runDispatcherB();
	}

	return 0;
}

void initializeSystem()
{
	// Postcondition: Helper processes were also created to run BBB's other
	// 		  tasks

	// Create a size five bounded buffer to hold users' delivered jobs
	
	// Initialize the system clock to zero

	// Create three helper processes to run different tasks of BBB
	for (helperProcessesReady = 0;
	     helperProcessesReady < 3;
	     helperProcessesReady++)
	{
		// Create a new helper process
		fork();

		// If the current process is a child process, provide a number
		// to the child process and leave the for loop
		if (pidOfChild == 0)
		{
			// Receive a new process number
			processNumber = helperProcessesReady + 1;

			// Leave the for loop
			break;
		}
	}
	
}

void runClock()
{
	// Postcondition: The simulation's clock was managed

	// Handle the simulation's clock until the simulation is instructed to
	// end

}

void runCPU()
{
	// Postcondition:

	// Sleep for the current job's requested sleep length
	int neededLength;
	sleep(neededLength);
}

void runDispatcherA()
{
	// Postcondition:
 
}

void runDispatcherB()
{
	// Postcondition:

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
