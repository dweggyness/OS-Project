#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/socket.h> // header for socket specific functions and macros declarations
#include <netinet/in.h> //header for MACROS and structures related to addresses "sockaddr_in", INADDR_ANY 
#include <arpa/inet.h> // header for functions related to addresses from text to binary form, inet_pton 
#include <pthread.h> // header for thread functions declarations: pthread_create, pthread_join, pthread_exit
#include <signal.h> // header for signal related functions and macros declarations
// compile your code with: gcc -o output code.c -lpthread

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
  "whoami\n\n"
  "type \"exit\" to quit the program\n";
  send(socket, welcomeMessage, strlen(welcomeMessage), 0);

  // variable such as message buffers to receive and send messages
  
  while (1) {
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
      char message[1024] = {0};
      recv(socket, message, sizeof(message),0); 

      printf("Received command: %s \n", message);  
      message[strcspn(message, "\n")] = 0;
      
      //handle exit command
      if (strcmp(message, "exit") == 0){ 
        printf("Client exited. Terminating session... \n");
        close(socket);

        char* message = "exit";
        
        write(fd[1], message, 1024);
        exit(EXIT_SUCCESS);
      }

      char message_copy[1024];
      strcpy(message_copy, message);
      char* message_split = strtok(message_copy, " "); //return a pointer

      //handle commands with only blanks
      if (message_split == NULL) { 
        printf("empty cmd \n");
        char* errMessage = "No command entered. Continue... \n";
        
        write(fd[1], errMessage, 1024);
        close(fd[1]);  
        close(fd[0]);  
        close(socket);  

        exit(EXIT_SUCCESS);
      }

      // update if-else conditions and sent -> write
      if ((strcmp(message_split, "cat") != 0) && (strcmp(message_split, "sort") != 0)
        && (strcmp(message_split, "sample") != 0) && (strcmp(message_split, "df") != 0) && 
        (strcmp(message_split, "ls") != 0) && (strcmp(message_split, "man") != 0) &&
        (strcmp(message_split, "pwd") != 0) && (strcmp(message_split, "echo") != 0) &&
        (strcmp(message_split, "ps") != 0) && (strcmp(message_split, "whoami") != 0) &&
        (strcmp(message_split, "./Test.o") != 0)) { 
        
        printf("invalid commands \n");

        // write invalid message to the pipe
        char* errMessage = "Command is currently unavailable, change one... \n";
        write(fd[1], errMessage, 1024);

        close(fd[1]);  
        close(fd[0]);  
        close(socket);  
        exit(EXIT_SUCCESS);
      }  

      // redirect STDOUT to sock2 , before calling the execvp for valid commands

      dup2(fd[1], STDOUT_FILENO);  /* duplicate socket on stdout */
      dup2(fd[1], STDERR_FILENO);  /* duplicate socket on stderr too */
      close(fd[1]);  
      close(fd[0]);  
      close(socket);  

      readParseInput(message);
      exit(EXIT_SUCCESS);

    } else { //parent process under a thread, run only when input is "exit"
      close(fd[1]);  
      //wait(NULL);

      char buf[1024] = {0};
      int nread = read(fd[0], buf, 1024);

      // if command is to exit, we exit
      if (strcmp(buf, "exit") == 0) {
        pthread_exit(NULL);
      }

      if (nread > 0) {
        printf("Sending Valid Buffer \n\n");
        send(socket, &buf, sizeof(buf), 0);
      } else if (nread == 0) { // read from pipe, but its empty. pipe returned no output
        printf("Sending Empty Buffer \n\n");
        send(socket, "", sizeof(""), 0); // send an empty string
      }
      close(fd[0]);
    }
  }
  
  pthread_exit(NULL);// terminate the thread
}



int main()
{ 
    int socket_fd, new_socket_fd;
    struct sockaddr_in address;
    pthread_attr_t pthread_attr;
    pthread_t pthread;
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
      /* TODO: Initialise arguments passed to threads here. See lines 22 and
        * 139.
        */

      /* Create thread to serve connection to client. */
      if (pthread_create(&pthread, &pthread_attr, HandleClient, (void *)pthread_arg) != 0) {
          perror("pthread_create");
          free(pthread_arg);
          continue;
      }
    }
    
    return 0;
}