#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <semaphore.h>

int main(int argc, char *argv[])
{
  if (argc != 3) {
    printf("%s \n", argv[1]);
    printf("argc: %d, Please enter 2 parameters (jobTime threadID) \n", argc);
    return -1;
  }

  int jobTimeRemaining = atoi(argv[1]);
  long threadID = atol(argv[2]);;

  sem_t *semaphore;
  semaphore = sem_open("/dummyProgramSemaphore", O_CREAT, 0644, 1);
  while(1) {
    int semaphore_val = 1;
    int returnValue = sem_getvalue(semaphore, &semaphore_val);

    // returnValue should be 0 if the getvalue call was successful
    if (semaphore_val == 1) {
      jobTimeRemaining--;
      printf("Thread ID: %ld, running for an iteration. Remaining time: %d \n", threadID, jobTimeRemaining);
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