#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

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

void readParseInput() {
  char inputStr[1000];

  char *firstcommand[10];
  char *secondcommand[10];
  char *thirdcommand[10];
  char *fourthcommand[10];

  int firstIndex = 0, secondIndex = 0, thirdIndex = 0, fourthIndex = 0;

  //create our own shell
  printf("$ ");
  fgets(inputStr, sizeof(inputStr), stdin);
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
      pid_t pid = fork();
      if(pid < 0){
        exit(EXIT_FAILURE);
      }
      else if(pid == 0){
        readParseInput();
      }
      else{
        wait(NULL);
        continue;
      }
    }
    
    return 0;
}