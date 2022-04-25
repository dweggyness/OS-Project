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
void* HandleClient(void* new_socket)
{

  pthread_detach(pthread_self()); // detach the thread as we don't need to synchronize/join with the other client threads, their execution/code flow does not depend on our termination/completion 
  int socket = *(int*)new_socket;
  free(new_socket);
  printf("handling new client in a thread using socket: %d\n", socket);
  printf("Listening to client..\n"); // while printing make sure to end your strings with \n or \0 to flush the stream, other wise if in anyother concurent process is reading from socket/pipe-end with standard input/output redirection, it will keep on waiting for stream to end. 
    
  // variable such as message buffers to receive and send messages
  
  char message[1024] = {0};
  recv(socket, message, sizeof(message),0); 

<<<<<<< Updated upstream
  printf("Received command: %s \n", message);  
  message[strcspn(message, "\n")] = 0;
  
  //handle exit command
  if (strcmp(message, "exit") == 0){ 
    printf("Client exited. Terminating session... \n");
    close(socket);
  }
=======
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
        pthread_exit(NULL);// terminate the thread
      }

      if (strcmp(message,"exit_client") == 0)
      {
        printf("closing the client communication socket : %d and terminating the corresponding thread. \n", socket);
        close(socket); // close the conneciton with client
        // break the infinite loop so that this thread could be terminated
         pthread_exit(NULL);// terminate the thread
      }
>>>>>>> Stashed changes


  char message_copy[1024];
  strcpy(message_copy, message);
  char* message_split = strtok(message_copy, " "); //return a pointer
  

<<<<<<< Updated upstream
  //handle commands with only blanks
  if (message_split == NULL) { 
    printf("empty cmd \n");
    char* errMessage = "No command entered. Continue... \n";
    send(socket, errMessage, strlen(errMessage), 0);

    exit(EXIT_SUCCESS);
=======
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
        continue;
      }  

      // redirect STDOUT to sock2 , before calling the execvp
      dup2(fd[1], STDOUT_FILENO);  /* duplicate socket on stdout */
      dup2(fd[1], STDERR_FILENO);  /* duplicate socket on stderr too */
      close(fd[1]);  
      close(fd[0]);  
      close(socket);  

      readParseInput(message);
      exit(EXIT_SUCCESS);

    } else {


      close(fd[1]);  
      wait(NULL);

      char buf[1024] = {0};
      int nread = read(fd[0], buf, 1024);

      if (nread > 0) {
        printf("Sending Valid Buffer \n\n");
        send(socket, &buf, sizeof(buf), 0);
      } else if (nread == 0) { // read from pipe, but its empty. pipe returned no output
        printf("Sending Empty Buffer \n\n");
        send(socket, "", sizeof(""), 0); // send an empty string
      }
      close(fd[0]);
    }
>>>>>>> Stashed changes
  }

  //limit the commands to the only listed 15

  /* sample command with 3 pipes */
  // cat words.txt | grep yasin | tee output1.txt | wc -l
  // cat words.txt | uniq | sort | head -10
  // sort alphabets.txt | head -10 | tail -n 5 | tee output3.txt

  /* sample command with 2 pipe */
  // sort words.txt | head -10 | grep 'a'
  // cat words.txt | grep yasin | wc -l

  /* sample command with 1 pipe */
  // cat alphabets.txt | tail -10
  // cat words.txt | uniq
  // df | tee disk_usage.txt

  /* sample command with 0 pipes */
  // cat alphabets.txt
  // ls -l
  // man
  // pwd
  // echo hello
  // ps
  // whoami


  // update if-else conditions and sent -> write
  if ((strcmp(message_split, "cat") != 0) && (strcmp(message_split, "sort") != 0)
    && (strcmp(message_split, "sample") != 0) && (strcmp(message_split, "df") != 0) && 
    (strcmp(message_split, "ls") != 0) && (strcmp(message_split, "man") != 0) &&
    (strcmp(message_split, "pwd") != 0) && (strcmp(message_split, "echo") != 0) &&
    (strcmp(message_split, "ps") != 0) && (strcmp(message_split, "whoami") != 0)){ 
    
    printf("invalid commands \n");
    char* errMessage = "Command is currently unavailable, change one... \n";
    send(socket, errMessage, strlen(errMessage), 0);
    // close(sock2);
    exit(EXIT_SUCCESS);
  }  

  // redirect STDOUT to sock2 , before calling the execvp
  dup2(socket, STDOUT_FILENO);  /* duplicate socket on stdout */
  dup2(socket, STDERR_FILENO);  /* duplicate socket on stderr too */
  close(socket);  /* can close the original after it's duplicated */

  readParseInput(message);
  
  pthread_exit(NULL);// terminate the thread
}



int main()
{ 
    signal(SIGINT, serverExitHandler);
    int sock1, sock2;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    //create socket file descriptor 
    if((sock1 = socket(AF_INET, SOCK_STREAM, 0)) == 0){
      perror("socket failed");
      exit(EXIT_FAILURE);
    }

    // setting the address to be bind to socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);


    // attaching socket to addresses (any/all local ip with port 5000)
    if (bind(sock1, (struct sockaddr *)&address, sizeof(address)) < 0) 
    // checking if bind fails
    {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }

    printf("Server successfully started at PORT %d \n", PORT);
    
    while(1){

      if (listen(sock1, 10) < 0) // defining for socket length of queue for pending client connections
      {
        perror("Listen Failed");
        exit(EXIT_FAILURE);
      }
      if ((sock2 = accept(sock1, (struct sockaddr *)&address,
       (socklen_t *)&addrlen)) < 0) // accepting the client connection with creation/return of a new socket for the established connection to enable dedicated communication (active communication on a new socket) with the client
      {
        perror("accept");
        exit(EXIT_FAILURE);
      }

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
        send(sock2, welcomeMessage, strlen(welcomeMessage), 0);

        //while (1) { //every client
     
        int rc; // return value from pthread_create to check if new thread is created successfukky                           */
        pthread_t  thread_id;  // thread's ID (just an integer, typedef unsigned long int) to indetify new thread
        int* new_socket = (int*)malloc(sizeof(int)); // for passing safely the integer socket to the thread
        if ( new_socket == NULL ) {
          fprintf(stderr, "Couldn't allocate memory for thread new socket argument.\n");
          exit(EXIT_FAILURE);
        }
        *new_socket = sock2;

        // create a new thread that will handle the communication with the newly accepted client
        rc = pthread_create(&thread_id, NULL, HandleClient, new_socket);  
        if(rc)      // if rc is > 0 imply could not create new thread 
        {
          printf("\n ERROR: return code from pthread_create is %d \n", rc);
          exit(EXIT_FAILURE);
        }
<<<<<<< Updated upstream

        //}
      }
      
      
=======
      //}
>>>>>>> Stashed changes
    }
    close(sock1);
    
    return 0;
}