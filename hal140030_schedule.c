/*
    HAILEMESKEL A LAKEW
    4/25/2017

    PROJECT 3


    MAKE SURE TO READ ALL COMMENTARY, AS FULL LOGIC WILL NOT BE APPARENT WITH JUST A GLANCE
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/time.h>

#include <sys/types.h>
#include <sys/file.h>

#include <ctype.h>

struct process // Data Structure used to represent a process
{
    int             ProcessID, // Presents Process ID
                    StartTime,
                    ServiceTime, // Service time value that will be changed in order to calculate future run times
                    ServiceTime2, // Service Time value that will not be changed in order to be used for in caclulation in main
                    ArrivalTime,
                    Turnaround;

    double          NormTurnaround; // Normalized Turnaround Time

    int             done; // Flag to show if process is done or not
};

void createProcesses(struct process *queue) // Function to create 100 random simulated processes
{
    struct timespec start, stop;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&start);  // Start timer

    //printf("Current Time: \t%lu\n", start.tv_nsec); // Inform current time

    time_t t;
    srand((unsigned)time (&t));// Seeded random generator

    int i = 0,
        numProcess = 100,
        prev = start.tv_nsec;

    for(i = 0; i < numProcess; i++) // Fill every process in list of processes
    {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&stop);  // Start timer
        queue[i].ProcessID = i + 1;
        queue[i].ServiceTime = 10 + (rand() % 990);
        queue[i].ServiceTime2 = queue[i].ServiceTime;
        queue[i].ArrivalTime = rand() & 1000;

        prev = prev + (rand() & 100);
        queue[i].ArrivalTime = prev - start.tv_nsec;

        queue[i].StartTime = -1; // Flag to show if process has started
        queue[i].done = 0;
    }
}

void SRTScheduler(struct process *queue)
{

    int curTime = 0; // Counter incremented every iterations, will denote what a ns is
    int working = 1;

    int spot = 0; // Used to iterate through all of the processes

    while(working) // Flag to show if all processes are not done
    {
                /* PLEASE READ ALL COMMENTS*/


                // The process returned by this if statement will NOT be the process to run
                // This if statemetn will only present something that can be compared below
                if(queue[spot].ServiceTime == 0) // If current process is finished, run another available process
                {
                    /** MAKE SURE TO READ ABOVE COMMENT!**/

                    //printf("Process %d has finished at Time :%dns\n", queue[spot].ProcessID, curTime);

                    // Calculate values as processes has finished and able to present relevant information
                    queue[spot].Turnaround = curTime - queue[spot].ArrivalTime;
                    queue[spot].NormTurnaround = (float)queue[spot].Turnaround / queue[spot].ServiceTime2;

                    queue[spot].done = 1; // Flag is done
                    spot = 0;

                    while(spot < 100) // Find next availble process based off of arrival time
                        if(queue[spot].done == 0 && queue[spot].ArrivalTime <= curTime)
                            break;
                        else
                            spot++;

                    if(spot == 100) // If the whole list is been scanned, flag to end program
                        return;
                }

                // Check if the current process is the least remaining time, if not replace
                int i = 0;
                for(i = 0; i< 100; i++)
                    if(curTime >= queue[i].ArrivalTime && (queue[i].done == 0)) // Make sure process is running and not finsihed yet
                    {
                        if(queue[i].ServiceTime < queue[spot].ServiceTime) // Keep track of least remaining time process
                        {
                            //printf("\tProcess %d was stopped at Time :%dns\n", queue[spot].ProcessID, curTime);
                            //printf("Process %d is now running at Time :%dns\n", queue[i].ProcessID, curTime);

                            spot = i; // Assign new least reaming time process
                        }
                    }

            if(queue[spot].StartTime == -1) // If process hasnt started yet, assign it the current running time as start time
                queue[spot].StartTime = curTime;

        queue[spot].ServiceTime= queue[spot].ServiceTime -1; // Decrement first copy so to flag if the current process is finsiehd
        curTime++; // INcrement time counter
    }
}

/********READ ABOVE********/
void LRTScheduler(struct process *queue)
{

    int curTime = 0;
    int working = 1;

    int spot = 0;

    while(working)
    {
                if(queue[spot].ServiceTime == 0) // Makes sure CPU is not idle to the best of ablility
                {
                    //printf("Process %d has finished at Time :%dns\n", queue[spot].ProcessID, curTime);

                    queue[spot].Turnaround = curTime - queue[spot].ArrivalTime;
                    queue[spot].NormTurnaround = (float)queue[spot].Turnaround / queue[spot].ServiceTime2;

                    queue[spot].done = 1;
                    spot = 0;

                    while(spot < 100)
                        if(queue[spot].done == 0 && queue[spot].ArrivalTime <= curTime)
                            break;
                        else
                            spot++;

                    if(spot == 100)
                        return;
                }

                int i = 0;
                for(i = 0; i< 100; i++)
                    if(curTime >= queue[i].ArrivalTime && (queue[i].done == 0))
                    {
                        if(queue[i].ServiceTime > queue[spot].ServiceTime) // Get process with most time remaining
                        {
                            if(curTime%5 == 0) // ONly update current process based off of time quantum
                            {
                                //printf("\tProcess %d was stopped at Time :%dns\n", queue[spot].ProcessID, curTime);
                                    //printf("Process %d is now running at Time :%dns\n", queue[i].ProcessID, curTime);

                                spot = i;
                            }
                        }
                    }

        if(queue[spot].StartTime == -1)
            queue[spot].StartTime = curTime;


        queue[spot].ServiceTime= queue[spot].ServiceTime -1;
        curTime++;
    }
}

void runProg()
{
    double  AverageTurnaroundTime = 0,
            AverageNormTurnaroundTime = 0,
            AverageServiceTime = 0,
            AverageCPU = 0;
	int i = 0; // For the Loop

    struct process queue[100]; // Array of Processes
    struct process queue2[100];

    createProcesses(queue); // Create Random Processes
    createProcesses(queue2);

    //for (int i = 0; i < 100; i++)
        //printf("Process %d with\t\tServiceTime: %dns\tArrivalTime: %dns after start\n", queue[i].ProcessID, queue[i].ServiceTime, queue[i].ArrivalTime);

    SRTScheduler(queue); // Run scheduler

    for(i=0; i<100; i++)
    {
        AverageTurnaroundTime = AverageTurnaroundTime + queue[i].Turnaround;
        AverageNormTurnaroundTime = AverageNormTurnaroundTime + queue[i].NormTurnaround;
        AverageServiceTime = AverageServiceTime+ queue[i].ServiceTime2;
    }

    AverageTurnaroundTime = AverageTurnaroundTime / 100;
    AverageNormTurnaroundTime= AverageNormTurnaroundTime / 100;
    AverageServiceTime = AverageServiceTime / 100;
    AverageCPU = (1 - (AverageServiceTime / AverageTurnaroundTime)) * 100;

    // Print Resulrs
    printf("SRT Scheduler Results\n\tProcessor Utilization: %f%%\n\tAverage Turnaround Time: %f\n\tAverage Normalized Turnaround Time: %f\n", AverageCPU, AverageTurnaroundTime, AverageNormTurnaroundTime);


    // Same as above
    LRTScheduler(queue2);

    AverageTurnaroundTime = 0;
    AverageNormTurnaroundTime = 0;
    AverageServiceTime = 0;
    AverageCPU = 0;

    for(i=0; i<100; i++)
    {
        AverageTurnaroundTime = AverageTurnaroundTime + queue2[i].Turnaround;
        AverageNormTurnaroundTime = AverageNormTurnaroundTime + queue2[i].NormTurnaround;
        AverageServiceTime = AverageServiceTime+ queue2[i].ServiceTime2;
    }

    AverageTurnaroundTime = AverageTurnaroundTime / 100;
    AverageNormTurnaroundTime= AverageNormTurnaroundTime / 100;
    AverageServiceTime = AverageServiceTime / 100;
    AverageCPU = (1 - (AverageServiceTime / AverageTurnaroundTime)) * 100;

    printf("LRT Scheduler Results\n\tProcessor Utilization: %f%%\n\tAverage Turnaround Time: %f\n\tAverage Normalized Turnaround Time: %f\n", AverageCPU, AverageTurnaroundTime, AverageNormTurnaroundTime);

}

int main()
{
        printf("/* --> IF NUMBERS ARE NEGATIVE OR EXTENSIVE, PLEASE RUN PROGRAM AGAIN\n");
        printf("/* --> PLEASE READ COMMENTARY OF CODE\n");
        printf("/* --> IF IN NEED FOR MORE INFORMATION, UMCOMMENT PRINTF CODE IN SOURCE\n");
        runProg();
}
