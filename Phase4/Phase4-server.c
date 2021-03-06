#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>
#include <fcntl.h> 
#include <semaphore.h>
#include <limits.h> // INT_MAX
#include <sys/socket.h> // header for socket specific functions and macros declarations
#include <netinet/in.h> //header for MACROS and structures related to addresses "sockaddr_in", INADDR_ANY 
#include <arpa/inet.h> // header for functions related to addresses from text to binary form, inet_pton 
#include <pthread.h> // header for thread functions declarations: pthread_create, pthread_join, pthread_exit
#include <signal.h> // header for signal related functions and macros declarations
// compile your code with: gcc -o output code.c -lpthread

#define SHARED 1

//Size of each chunk of data received
#define CHUNK_SIZE 512


// A linked list node for process queue
struct Node {
    long threadID;
    int jobTimeRemaining;
    int roundNumber;
    sem_t* semaphore; 
    struct Node* next;
};

// sonya original code
// struct Node* head = NULL;
// head = (struct Node*)malloc(sizeof(struct Node));
// struct Node* permanentHead = head;

// i move the code to main() because you can't malloc global variable
// CTRL+F "@@HELLOSONYA@@" to find the code
struct Node* head = NULL;

int newProcessCreated = 0;

void insertIntoList(struct Node* node) {
  node->next = head;
  head = node;
}

// function to check the current smallest round number
// int getSmallestRound(){
//   struct Node* temp = head;
//   int minRound = INT_MAX;
//   while (temp != NULL) {
//     if(temp->roundNumber < minRound){  
//       minRound = temp->roundNumber;
//     }
//     temp = temp->next;
//   }
//   return minRound;
// }

// function to get job with smallest time remaining in the current round 
struct Node* getSmallestJob(struct Node* currentlyRunningThread){

  struct Node* temp = head;

  int min = INT_MAX;
  long minID;

  //int smallestRound = getSmallestRound();
  //printf("round number for the current running process: %d \n", smallestRound);

  while (temp != NULL) {
    if(temp->jobTimeRemaining < min){  
      if(currentlyRunningThread == NULL || currentlyRunningThread->threadID != temp->threadID){
        min = temp->jobTimeRemaining;
        minID = temp->threadID;
      }

    }
    temp = temp->next;
  }

  temp = head;
  // minID = threadID with the lowest jobTimeRemaining
  while(temp != NULL){
    if(temp->threadID == minID){
      return temp;
    }
    temp = temp->next;
  }

  return currentlyRunningThread; //when there is no other threads besides current one
}

// function to delete - given a threadID, delete that node from the linked list
// use pointer to pointer (reference)
void deleteNode(long id){

  struct Node *temp = head, *prev;

  if (temp != NULL && temp->threadID == id) {
    //printf("Yes its this one \n");
    head = temp->next; // Changed head
    free(temp); // free old head
    return;
  }

  // Search for the threadID to be deleted, keep track of the previous node 
  while (temp != NULL && temp->threadID != id) {
      prev = temp;
      temp = temp->next;
  }

  // If threadID was not present in linked list
  if (temp == NULL)
      return;

  // Unlink the node from linked list
  prev->next = temp->next;

  // free memory
  free(temp);
}

// function to get - given a threadID, return the node object
struct Node* getNode(long id){
  struct Node *temp = head;

  while(temp != NULL){
    if(temp->threadID == id){
      return temp;
    }
    temp = temp->next;
  }
  return head;
}

// function that returns true if the linked list is empty
bool isEmpty(){
  if (head == NULL) {
    return true;
  } 
  return false;
  // return (head->next == NULL); //head itself is dummy node
}


void *SchedulerFunction() {
  sem_t *programSemaphore;
  programSemaphore = sem_open("/dummyProgramSemaphore", O_CREAT, 0644, 0);

  int quantum = 0;
  struct Node* currentlyRunningThread; // current node/thread
  while(1) {
    sleep(1);
    // printf("getting head in scheduler: %ld \n", head->threadID);
    // if process queue is empty, then early return
    
    // quantum has ended, stop the current thread and select next process
    // OR, a new process has been created. forcefully select a new thread to execute
    if (quantum <= 0 || newProcessCreated == 1) { 
      if (isEmpty()) continue; 
      
      if (quantum <= 0) {
        printf("Quantum ended. \n");
      } else if (newProcessCreated == 1) {
        printf("New process entered process queue");
      }
      newProcessCreated = 0;
    
      printf("Attempting to schedule new task... \n");
      // stop currently running node, if it exists
      if (currentlyRunningThread != NULL) {
        sem_t *currentSemaphore = currentlyRunningThread->semaphore; // currentRunningThread.semaphore);
        sem_wait(programSemaphore); // stop currently running dummy program
        sem_wait(currentSemaphore); // stop currently running semaphore
      }

      // select the next node to run except the current node
      struct Node* nextThread = getSmallestJob(currentlyRunningThread); // get job with smallest remaining time

      sem_t *threadSemaphore = nextThread->semaphore; 
      sem_post(programSemaphore); // allow dummyprogram to run
      sem_post(threadSemaphore); // release the semaphore for the thread, it is now running

      currentlyRunningThread = nextThread;

      int threadRoundNum = nextThread->roundNumber; // depending on round number, different quantum
      switch(threadRoundNum) {
        case 1: 
          quantum = 3;
          break;
        case 2:
          quantum = 7;
          break;
        default:
          quantum = 3;
          if(threadRoundNum > 2) {
            quantum = 10;
          }
          break;
      }

      printf("Scheduled new task %ld. Given a quantum of: %d \n", nextThread->threadID, quantum);
    } else { // quantum > 0, keep running
      quantum--;
    }
  }
}




// sem_t *semaphore;
// semaphore = sem_open("/dummyProgramSemaphore", O_CREAT, 0644, 1);

typedef struct pthread_arg_t {
    int new_socket_fd;
    struct sockaddr_in client_address;
} pthread_arg_t;

#define NUM_CLIENTS 10
#define PORT 5000


// function routine of Signal Handler for SIGINT, to terminate all the threads which will all be terminated as we are calling exit of a process instead of pthread_exit
void serverExitHandler(int sig_num)
{
    printf("\n server exit  \n");
    sem_unlink("/dummyProgramSemaphore");
    fflush(stdout);// force to flush any data in buffers to the file descriptor of standard output,, a pretty convinent function
    exit(0);
}

void processpipeline1CMD(char *firstcommand[])
{
  execvp(firstcommand[0], firstcommand);
  fprintf(stderr, "Execvp failed while executing %s \n", firstcommand[0]);
  exit(EXIT_FAILURE);
}

void processpipeline2CMD(char *firstcommand[], char *secondcommand[])
{
  int fd1[2]; // pipe 1 for getting output from child 1 and giving it to child 2 also
  pid_t pid;
  

  if (pipe(fd1) < 0)
      exit(EXIT_FAILURE);

  pid = fork();
  if (pid < 0)
      exit(EXIT_FAILURE);
  else if (pid == 0) // child 1 for cmd 1
  {
    dup2(fd1[1], 1);// write by redirecting standard output to pipe 1
    close(fd1[1]);
    close(fd1[0]);
    execvp(firstcommand[0], firstcommand);
    fprintf(stderr, "Execvp failed while executing %s \n", firstcommand[0]);
    exit(EXIT_FAILURE);
  }
  else
  {    // parent for cmd 2
    dup2(fd1[0], 0); // reading redirected ouput of ls through pipe 1
    close(fd1[1]);
    close(fd1[0]);
    execvp(secondcommand[0], secondcommand);
    fprintf(stderr, "Execvp failed while executing %s \n", secondcommand[0]);
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "Reached unexpectedly\n");
  exit(EXIT_FAILURE);
}

void processpipeline3CMD(char *firstcommand[], char *secondcommand[] , char *thirdcommand[])
{
  int fd1[2]; // pipe 1 for getting output from child 1 and giving it to child 2 also
  int fd2[2]; // pipe 2 for getting output from child 2 and giving it to child 3
  pid_t pid;
  

  if (pipe(fd1) < 0)
      exit(EXIT_FAILURE);
  if (pipe(fd2) < 0)
      exit(EXIT_FAILURE);

  pid = fork();
  if (pid < 0)
      exit(EXIT_FAILURE);
  else if (pid == 0) // child 1 for cmd 1
  {
      dup2(fd1[1], 1);// write by redirecting standard output to pipe 1
      close(fd1[1]);
      close(fd1[0]);
      close(fd2[0]);
      close(fd2[1]);
      execvp(firstcommand[0], firstcommand);
      fprintf(stderr, "Execvp failed while executing %s \n", firstcommand[0]);
      exit(EXIT_FAILURE);
  }
  else
  {   
      pid = fork();
      if (pid == 0) // child 2 for cmd 2
      {
        dup2(fd1[0], 0); // reading redirected ouput of ls through pipe 1
        dup2(fd2[1], 1); // write by redirecting standard output to pipe 2
        close(fd1[1]);
        close(fd1[0]);
        close(fd2[1]);
        close(fd2[0]);
        execvp(secondcommand[0], secondcommand);
        fprintf(stderr, "Execvp failed while executing %s \n", secondcommand[0]);
        exit(EXIT_FAILURE);
      }
      else // parent, for cmd 3
      {   
        dup2(fd2[0], 0); // reading redirected ouput of ls through pipe 1
        close(fd1[1]);
        close(fd1[0]);
        close(fd2[1]);
        close(fd2[0]);
        execvp(thirdcommand[0], thirdcommand);
        fprintf(stderr, "Execvp failed while executing %s \n", thirdcommand[0]);
        exit(EXIT_FAILURE);
      }
  }
  fprintf(stderr, "Reached unexpectedly\n");
  exit(EXIT_FAILURE);
}

void processpipeline4CMD(char *firstcommand[], char *secondcommand[] , char *thirdcommand[], char *fourthcommand[])
{
  int fd1[2]; // pipe 1 for getting output from child 1 and giving it to child 2 also
  int fd2[2]; // pipe 2 for getting output from child 2 and giving it to child 3
  int fd3[2]; // pipe 3 for getting output from cmd 3, and giving it to cmd 4
  pid_t pid;
  

  if (pipe(fd1) < 0)
      exit(EXIT_FAILURE);
  if (pipe(fd2) < 0)
      exit(EXIT_FAILURE);
  if (pipe(fd3) < 0)
      exit(EXIT_FAILURE);

  pid = fork();
  if (pid < 0)
      exit(EXIT_FAILURE);
  else if (pid == 0) // child 1 for cmd 1
  {
      dup2(fd1[1], 1);// write by redirecting standard output to pipe 1
      close(fd1[1]);
      close(fd1[0]);
      close(fd2[0]);
      close(fd2[1]);
      close(fd3[0]);
      close(fd3[1]);
      execvp(firstcommand[0], firstcommand);
      fprintf(stderr, "Execvp failed while executing %s \n", firstcommand[0]);
      exit(EXIT_FAILURE);
  }
  else
  {   
      pid = fork();
      if (pid == 0) // child 2 for cmd 2
      {
          dup2(fd1[0], 0); // reading redirected ouput of ls through pipe 1
          dup2(fd2[1], 1); // write by redirecting standard output to pipe 2
          close(fd1[1]);
          close(fd1[0]);
          close(fd2[1]);
          close(fd2[0]);
          close(fd3[1]);
          close(fd3[0]);
          execvp(secondcommand[0], secondcommand);
          fprintf(stderr, "Execvp failed while executing %s \n", secondcommand[0]);
          exit(EXIT_FAILURE);
      }
      else 
      {   
          pid = fork();
          if (pid == 0) // child 3 for cmd 3 
          {
            dup2(fd2[0], 0); // reading redirected ouput of ls through pipe 1
            dup2(fd3[1], 1); // write by redirecting standard output to pipe 2
            close(fd1[1]);
            close(fd1[0]);
            close(fd2[1]);
            close(fd2[0]);
            close(fd3[0]);
            close(fd3[1]);
            execvp(thirdcommand[0], thirdcommand);
            fprintf(stderr, "Execvp failed while executing %s \n", thirdcommand[0]);
            exit(EXIT_FAILURE);

          }
          else { // parent for cmd 4
            dup2(fd3[0], 0);
            close(fd1[1]);
            close(fd1[0]);
            close(fd2[1]);
            close(fd2[0]);
            close(fd3[0]);
            close(fd3[1]);
            execvp(fourthcommand[0], fourthcommand);
            fprintf(stderr, "Execvp failed while executing %s \n", fourthcommand[0]);
            exit(EXIT_FAILURE);
          }
      }
  }
  fprintf(stderr, "Reached unexpectedly\n");
  exit(EXIT_FAILURE);
}

// parser for input, supports up to 3 pipes ( 4 commands )

void readParseInput(char* inputStr) {
  char *firstcommand[10];
  char *secondcommand[10];
  char *thirdcommand[10];
  char *fourthcommand[10];

  int firstIndex = 0, secondIndex = 0, thirdIndex = 0, fourthIndex = 0;

  // remove trailing \n from the inputStr
  // https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
  inputStr[strcspn(inputStr, "\n")] = 0;

  // input string done preprocessing

  char* strtok1;
  char* strtok2;

  char* command = strtok_r(inputStr, "|", &strtok1); // split the string by pipes

  int curCommand = 0;
  while (command) {

    if (strcmp(command, "|") == 0) {
      command = strtok_r(NULL, " ", &strtok1);
      continue;
    }
    curCommand++;
  
    
    char* strtok2 = NULL;
    char* token = strtok_r(command, " ", &strtok2); // split the command by spaces
    while (token) {
      // Strips a token of any quotation marks, and '\' e.g 'yasin' -> yasin
      // code from : https://stackoverflow.com/questions/7143878/how-to-remove-quotes-from-a-string-in-c
      int j = 0;
      for (int i = 0; i < strlen(token); i ++) {
        if (token[i] != '\'' && token[i] != '\"' && token[i] != '\\') { 
            token[j++] = token[i];
        } else if (token[i+1] == '\'' && token[i+i] != '\"' && token[i] == '\\') { 
            token[j++] = '\'';
        } else if (token[i+1] != '\'' && token[i+i] != '\"' && token[i] == '\\') { 
            token[j++] = '\\';
        }
      }
      
      if(j>0) token[j]=0;
      switch(curCommand) { // add the token to the correct command order
        case 1:
          firstcommand[firstIndex] = token;
          firstIndex++;
          break;
        case 2:
          secondcommand[secondIndex] = token;
          secondIndex++;
          break;
        case 3:
          thirdcommand[thirdIndex] = token;
          thirdIndex++;
          break;
        case 4:
          fourthcommand[fourthIndex] = token;
          fourthIndex++;
          break;
      }

      token = strtok_r(NULL, " ", &strtok2);
    }

    // command construction complete, add a NULL to the end of the array
    switch(curCommand) { 
      case 1:
        firstcommand[firstIndex] = NULL;
        break;
      case 2:
        secondcommand[secondIndex] = NULL;
        break;
      case 3:
        thirdcommand[thirdIndex] = NULL;
        break;
      case 4:
        fourthcommand[fourthIndex] = NULL;
        break;

    }
    // go to next token
    command = strtok_r(NULL, "|", &strtok1);
  }

  // now, depending on the number of pipes, call processpipeline with different parameters
  // if only 2 commands, e.g "ls -l | wc -l", then thirdCommand = NULL and fourthCommand = NULL
  // since they are empty.

  switch (curCommand) {
    case 1: // only 1 command, the rest is NULL
      processpipeline1CMD(firstcommand);
      break;
    case 2:
      processpipeline2CMD(firstcommand, secondcommand);
      break;
    case 3:
      processpipeline3CMD(firstcommand, secondcommand, thirdcommand);
      break;
    case 4:
      processpipeline4CMD(firstcommand, secondcommand, thirdcommand, fourthcommand);
      break;
  }
}


// Function that handles the client
void* HandleClient(void* arg)
{
  pthread_arg_t *pthread_arg = (pthread_arg_t *)arg;
  int socket = pthread_arg->new_socket_fd;
  free(arg);
  printf("handling new client in a thread using socket: %d\n", socket);

  sem_t* clientSemaphore = malloc(sizeof(sem_t));
  sem_init(clientSemaphore, SHARED, 0); // initialize a locked semaphore
  
  // initial message sent to client after successful socket connection
  char* welcomeMessage = "@@@ Welcome to JS (Jun Sonya) Shell @@@\n"
  "Please type your command into the shell and limit your commands to the below 15\n"
  "\n"
  "Ideas for commands: \n"
  "--- 3 pipes --- \n"
  "cat words.txt | grep yasin | tee output1.txt | wc -l\n"
  "cat words.txt | uniq | sort | head -10\n"
  "sort alphabets.txt | head -10 | tail -n 5 | tee output3.txt\n"
  "--- 2 pipes --- \n"
  "sort words.txt | head -10 | grep 'a'\n"
  "cat words.txt | grep yasin | wc -l\n"
  "--- 1 pipe --- \n"
  "cat alphabets.txt | tail -10\n"
  "cat words.txt | uniq\n"
  "df | tee disk_usage.txt\n"
  "--- 0 pipes --- \n"
  "cat alphabets.txt\n"
  "ls -l\n"
  "man\n"
  "pwd\n"
  "echo hello\n"
  "ps\n"
  "./dummyProgram.o X(an integer)\n"
  "whoami\n\n"
  "type \"exit\" to quit the program\n";
  send(socket, welcomeMessage, strlen(welcomeMessage), 0);

  // variable such as message buffers to receive and send messages

  int isRunningDummyProgram = 0;
  
  while (1) {
    char message[4096] = {0};
    if (isRunningDummyProgram == 0) {
      // read input from user
      recv(socket, message, sizeof(message),0); 

      printf("Received command: %s \n", message);  
      message[strcspn(message, "\n")] = 0;
      
      //handle exit command
      if (strcmp(message, "exit") == 0){ 
        printf("Client exited. Terminating session... \n");
        close(socket);

        char* message = "exit";
        send(socket, &message, sizeof(message), 0);
        pthread_exit(NULL);
      }

      char message_copy[4096];
      strcpy(message_copy, message);
      char* message_split = strtok(message_copy, " "); //return a pointer

      //handle commands with only blanks
      if (message_split == NULL) { 
        printf("empty cmd \n");
        char errMessage[4096] = "No command entered. Continue... \n";
        send(socket, errMessage, strlen(errMessage), 0);
        continue;
      }

      if ((strcmp(message_split, "cat") != 0) && (strcmp(message_split, "sort") != 0)
        && (strcmp(message_split, "sample") != 0) && (strcmp(message_split, "df") != 0) && 
        (strcmp(message_split, "ls") != 0) && (strcmp(message_split, "man") != 0) &&
        (strcmp(message_split, "pwd") != 0) && (strcmp(message_split, "echo") != 0) &&
        (strcmp(message_split, "ps") != 0) && (strcmp(message_split, "whoami") != 0) &&
        (strcmp(message_split, "./dummyProgram.o") != 0)) { 
        
        printf("invalid commands \n");

        // write invalid message to the pipe
        char errMessage[4096] = "Command is currently unavailable, change one... \n";
        send(socket, errMessage, strlen(errMessage), 0);
        continue;
      }  

      //handle dummy program with process queue
      if (strcmp(message_split, "./dummyProgram.o") == 0){ 
        char *jobRemainingStr = strtok(NULL, " "); 
        
        if (jobRemainingStr == NULL) {
          char errMessage[4096] = "./dummyProgram.o has to be called with a job time parameter. \n";
          send(socket, errMessage, strlen(errMessage), 0);
          continue;
        }

        int jobTime = atoi(jobRemainingStr);

        struct Node* process = NULL;
        process = (struct Node*)malloc(sizeof(struct Node));

        process->threadID = pthread_self();
        process->jobTimeRemaining = jobTime;
        process->roundNumber = 1;
        process->semaphore = clientSemaphore; 
        
        // add to scheduler

        // insert the node into the process queue
        newProcessCreated = 1; // set global flag
        insertIntoList(process);

        isRunningDummyProgram = 1;
        // replace the "message" or command that will be executed in the child process
        memset(message, 0, 4096);
        sprintf(message, "./dummyProgram.o %d %ld", jobTime, process->threadID);

        // wait for scheduler to give the Go-Ahead to run the program.
        sem_wait(clientSemaphore);
      }
    } else if (isRunningDummyProgram == 1) {
      // get status of our process from the process queue,
      long threadID = pthread_self();
      struct Node* jobStatus = getNode(threadID);
      int jobTimeRemaining = jobStatus->jobTimeRemaining;
      memset(message, 0, 4096);
      sprintf(message, "./dummyProgram.o %d %ld", jobTimeRemaining, threadID);

      sleep(1);
      // still running the dummy program, wait for semaphore 
      // scheduler will unlock the semaphore when main thread is free
      sem_wait(clientSemaphore);
    }

    int fd[2]; // pipe 1 for getting output from child 1 and giving it to child 2
    
    if (pipe(fd) < 0) {
      printf("Pipe failed \n");
      exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if(pid < 0){
      printf("exit failure \n");
      exit(EXIT_FAILURE);
    }
    else if(pid == 0) {
      if (isRunningDummyProgram == 0) { // only dup STD out and STD in on non-program cmds
        dup2(fd[1], STDOUT_FILENO);  /* duplicate socket on stdout */
        dup2(fd[1], STDERR_FILENO);  /* duplicate socket on stderr too */
      }
      close(fd[1]);
      close(fd[0]);
      close(socket);  
      
      readParseInput(message);
      exit(EXIT_SUCCESS);
    } else { //parent process under a thread, run only when input is "exit"
      int waitStatus;
      wait(&waitStatus);
      close(fd[1]);  

      char buf[4096] = {0};
      int nread = read(fd[0], buf, 4096);

      // if command is dummyProgram, we need to handle return value
      if (isRunningDummyProgram) {
        int dummyRemainingTime = WEXITSTATUS(waitStatus);
        close(fd[0]);

        // here, get the job in the queue and update the remaining time/ round robin
        long threadID = pthread_self();
        struct Node* job = getNode(threadID);
        job->jobTimeRemaining = dummyRemainingTime;
        job->roundNumber = job->roundNumber + 1;

        if (dummyRemainingTime == 0) { // program done executing!
          char completeMessage[4096] = "./dummyProgram.o has successfully executed! \n";
          send(socket, completeMessage, strlen(completeMessage), 0);
          isRunningDummyProgram = 0;
          

          // delete the node from the process queue
          deleteNode(threadID);
        }

        // release the semaphore so that scheduler can take over control
        sem_post(clientSemaphore);
        // if remaining time 0 (finished), delete the node
        // processQueue.deleteNode(&head, pthread_self())
      } else {
        if (nread > 0) {
          printf("Sending Valid Buffer \n\n");
          send(socket, &buf, sizeof(buf), 0);
        } else if (nread == 0) { // read from pipe, but its empty. pipe returned no output
          printf("Sending Empty Buffer \n\n");
          send(socket, "", sizeof(""), 0); // send an empty string
        }
      }
      close(fd[0]);
      }
  }
}


int main()
{ 
    int socket_fd, new_socket_fd;
    struct sockaddr_in address;

    pthread_attr_t pthread_attr;
    pthread_t pthread;
    pthread_t mainThread;
    socklen_t client_address_len;

    /* Initialise IPv4 address. */
    memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    /* Create TCP socket. */
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    /* Assign signal handlers to signals. */
    if (signal(SIGINT, serverExitHandler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    /* Bind address to socket. */
    if (bind(socket_fd, (struct sockaddr *)&address, sizeof address) == -1) {
        perror("bind");
        exit(1);
    }

    /* Listen on socket. */
    if (listen(socket_fd, NUM_CLIENTS) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Server successfully started at PORT %d \n", PORT);

    /* Initialise pthread attribute to create detached threads. */
    if (pthread_attr_init(&pthread_attr) != 0) {
        perror("pthread_attr_init");
        exit(1);
    }
    if (pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED) != 0) {
        perror("pthread_attr_setdetachstate");
        exit(1);
    }

    pthread_arg_t *pthread_arg;


    // create a main thread for scheduling 
    /* Create thread to serve connection to client. */
    if (pthread_create(&mainThread, &pthread_attr, SchedulerFunction, NULL) != 0) {
        perror("pthread_create for main thread");
        free(pthread_arg);
    }

    while(1) {
      printf("New client! \n");
      pthread_arg = (pthread_arg_t *)malloc(sizeof *pthread_arg);
      if (!pthread_arg) {
          perror("malloc");
          continue;
      }

      /* Accept connection to client. */
      client_address_len = sizeof pthread_arg->client_address;
      new_socket_fd = accept(socket_fd, (struct sockaddr *)&pthread_arg->client_address, &client_address_len);
      if (new_socket_fd == -1) {
          perror("accept");
          free(pthread_arg);
          continue;
      }

      /* Initialise pthread argument. */
      pthread_arg->new_socket_fd = new_socket_fd;

      /* Create thread to serve connection to client. */
      if (pthread_create(&pthread, &pthread_attr, HandleClient, (void *)pthread_arg) != 0) {
          perror("pthread_create");
          free(pthread_arg);
          continue;
      }

    }
    
    return 0;
}