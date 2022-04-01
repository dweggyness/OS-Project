#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/socket.h> // header for socket specific functions and macros declarations
#include <netinet/in.h> //header for MACROS and structures related to addresses "sockaddr_in", INADDR_ANY 
#include <arpa/inet.h> // header for functions related to addresses from text to binary form, inet_pton 

#define PORT 8000


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
  //char inputStr[1000];

  char *firstcommand[5];
  char *secondcommand[5];
  char *thirdcommand[5];
  char *fourthcommand[5];

  int firstIndex = 0, secondIndex = 0, thirdIndex = 0, fourthIndex = 0;

  //create our own shell
  printf("$ ");
  //fgets(inputStr, sizeof(inputStr), stdin);
  inputStr[strcspn(inputStr, "\n")] = 0;
  printf("\n");
  if (strcmp(inputStr, "") == 0) { //handle empty command
    printf("No command entered. Continue... \n");
    //kill(getppid(), SIGUSR1); //kill the parent process within a child
    exit(0);
  }
  if(strcmp(inputStr, "exit") == 0){ //handle exit command
    printf("Exit program. Terminating... \n");
    kill(getppid(), SIGUSR1); //kill the parent process within a child
    exit(0);
  }

  char* token = strtok(inputStr, " ");

  int curCommand = 1;
  while (token) { // read the string, token by token
      
      // Strips a token of any quotation marks, e.g 'yasin' -> yasin
      // code from : https://stackoverflow.com/questions/7143878/how-to-remove-quotes-from-a-string-in-c
      int j = 0;
      for (int i = 0; i < strlen(token); i ++) {
        if (token[i] != '\'' && token[i] != '\\') { 
            token[j++] = token[i];
        } else if (token[i+1] == '\'' && token[i] == '\\') { 
            token[j++] = '\'';
        } else if (token[i+1] != '\'' && token[i] == '\\') { 
            token[j++] = '\\';
        }
      }

      if(j>0) token[j]=0;
      // strip done

      // now process the current token
      // if current token is '|', then its pipe. current CMD is done, move on to next command
      if (strcmp(token, "|") == 0) { // add a 'NULL' to the current cmd array, then move to next cmd
        switch(curCommand) { // otherwise, not a pipe command, just add the token to current cmd
          case 1:
            firstcommand[firstIndex] = NULL;
            curCommand++;
            break;
          case 2:
            secondcommand[secondIndex] = NULL;
            curCommand++;
            break;
          case 3:
            thirdcommand[thirdIndex] = NULL;
            curCommand++;
            break;
        }
      } else {
        switch(curCommand) { // otherwise, not a pipe command, just add the token to current cmd
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
    }
    // go to next token
    token = strtok(NULL, " ");
  }

  // end of string parsing
  // end of tokens, add a NULL to the final command array , e.g [ls, -l] -> [ls, -l, NULL]
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

int main()
{ 
    // sample command with 3 pipes
    // cat words.txt | grep yasin | tee output1.txt | wc -l
    // cat words.txt | uniq | sort | head -10
    // sort alphabets.txt | head -10 | tail -5 > output3.txt | cat output3.txt

    // sample command with 2 pipe
    // sort words.txt | head -10 | grep 'a'
    // cat words.txt | grep yasin | wc -l

    // sample command with 1 pipe
    // cat alphabets.txt | tail -10
    // cat words.txt | uniq
    
    // sample command with 0 pipes
    // ls -l
    // man
    

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


    // attaching socket to addresses (any/all local ip with port 5564)
    if (bind(sock1, (struct sockaddr *)&address,
     sizeof(address)) < 0) // checking if bind fails
    {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }


    printf("\n");
    printf("@@@ Welcome to JS (Jun Sonya) Shell @@@\n");
    printf("Please type your command into the shell \n");
    printf("\n");
    printf("Ideas for commands: \n");
    printf("--- 3 pipes --- \n");
    printf("cat words.txt | grep yasin | tee output1.txt | wc -l\n");
    printf("cat words.txt | uniq | sort | head -10\n");
    printf("sort alphabets.txt | head -10 | tail -n 5 | tee output3.txt\n");
    printf("--- 2 pipes --- \n");
    printf("sort words.txt | head -10 | grep 'a'\n");
    printf("cat words.txt | grep yasin | wc -l\n");
    printf("--- 1 pipe --- \n");
    printf("cat alphabets.txt | tail -10\n");
    printf("cat words.txt | uniq\n");
    printf("df | tee disk_usage.txt\n");
    printf("--- 0 pipes --- \n");
    printf("... You got this! \n\n");
    printf("type \"exit\" to quit the program\n");

    
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

      char message[1024] = {0};
     
      recv(sock2, message, sizeof(message),0); // receive input string from client
      printf("Server Received: %s\n", message); // print the received message 

      pid_t pid = fork();
      if(pid < 0){
        exit(EXIT_FAILURE);
      }
      else if(pid == 0){
        readParseInput(message);
      }
      else{
        wait(NULL);
        continue;
      }
    }
    
    return 0;
}