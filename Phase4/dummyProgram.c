#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>

int main(int argc, char *argv[])
{
  if (argc != 3) {
    printf("Please enter 2 parameters (threadID jobTime) \n");
    return -1;
  }

  int threadID = atoi(argv[1]);
  int jobTimeRemaining = atoi(argv[2]);

  sem_t *semaphore;
  semaphore = sem_open("/dummyProgramSemaphore", O_CREAT, 0644, 1);
  while(1) {
    // get semaphore 
    int semaphore_val = 1;
    int returnValue = sem_getvalue(semaphore, &semaphore_val);

    // returnValue should be 0 if the getvalue call was successful
    if (semaphore_val == 1) {
      jobTimeRemaining--;
      printf("Thread ID: %d, running for an iteration. Remaining time: %d \n", threadID, jobTimeRemaining);
      if (jobTimeRemaining <= 0) { // job complete
        return 0;
      }
      
      sleep(1);
    } else {
      return jobTimeRemaining;
    }
  }
  return 0;
}