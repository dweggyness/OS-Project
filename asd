#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void processpipeline(char *firstcommand[], char *secondcommand[] , char *thirdcommand[], char *fourthcommand[])
{
    int fd1[2]; // pipe 1 for getting output of ls from child 1 and giving it to child 2 also
    int fd2[2]; // pipe 2 for getting output of grep from child 2 and giving it to parent wc
    int fd3[2]; // pipe 3 for getting output from cmd 3, and giving it to cmd 4
    pid_t pid;
  
    if (pipe(fd1) < 0)
        exit(EXIT_FAILURE);
    if (pipe(fd2) < 0)
        exit(EXIT_FAILURE);
    if (pipe(fd3) < 0)
        exit(EXIT_FAILURE);

    printf(" pipeline %d: at work\n", getpid());
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    else if (pid == 0) // child 1 for cmd 1
    {
        printf("%d: child 1 - ls\n", getpid());

        dup2(fd1[1], 1);// write by redirecting standard output to pipe 1
        close(fd1[1]);
        close(fd1[0]);
        close(fd2[0]);
        close(fd2[1]);
        close(fd3[0]);
        close(fd3[1]);
        execvp(firstcommand[0], firstcommand);
        perror ("Execvp failed while executing grep");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("%d: parent - before second fork\n", getpid());
        if (secondcommand == NULL) { // no command 23, dont run command 2, 3 and 4
          exit(EXIT_SUCCESS);
        }

        pid = fork();
        if (pid == 0) // child 2 for cmd 2
        {
            printf("%d: child 2 - grep\n", getpid());
            if (thirdcommand == NULL) // no command after this, write to stdoutput
            {
              dup2(fd1[0], 0); // reading redirected ouput of ls through pipe 1
            } else { // cmd 4 after this, write to fd3
              dup2(fd1[0], 0); // reading redirected ouput of ls through pipe 1
              dup2(fd2[1], 1); // write by redirecting standard output to pipe 2
            }
            close(fd1[1]);
            close(fd1[0]);
            close(fd2[1]);
            close(fd2[0]);
            close(fd3[0]);
            close(fd3[1]);
            execvp(secondcommand[0], secondcommand);
            perror ("Execvp failed while executing grep");
            exit(EXIT_FAILURE);
        }
        else // parent 
        {
            if (thirdcommand == NULL) { // no command 3, dont run command 3 and 4
              exit(EXIT_SUCCESS);
            }
  
            printf("%d: parent - before second fork\n", getpid());
            pid = fork();
            if (pid == 0) // child 3 for cmd 3 
            {
              printf("%d: child 3 \n", getpid());
              if (fourthcommand == NULL) // no command after this, write to stdoutput
              {
                dup2(fd2[0], 0); // reading redirected ouput of ls through pipe 1
              } else { // cmd 4 after this, write to fd3
                dup2(fd2[0], 0); // reading redirected ouput of ls through pipe 1
                dup2(fd3[1], 1); // write by redirecting standard output to pipe 2
              }
              close(fd1[1]);
              close(fd1[0]);
              close(fd2[1]);
              close(fd2[0]);
              close(fd3[0]);
              close(fd3[1]);
              execvp(thirdcommand[0], thirdcommand);
              perror ("Execvp failed while executing grep");
              exit(EXIT_FAILURE);

            }
            else { // parent for cmd 4
              printf("%d: parent - wc\n", getpid());
              if (fourthcommand == NULL) {
                exit(EXIT_SUCCESS);
              }
              dup2(fd3[0], 0);
              close(fd1[1]);
              close(fd1[0]);
              close(fd2[1]);
              close(fd2[0]);
              close(fd3[0]);
              close(fd3[1]);
              execvp(fourthcommand[0], fourthcommand);
              perror ("Execvp failed while executing wc");
              exit(EXIT_FAILURE);
            }
        }
    }
    fprintf(stderr, "Reached unexpectedly\n");
    exit(EXIT_FAILURE);
}


void readParseInput() {
  char inputStr[1000];

  char *firstcommand[5];
  char *secondcommand[5];
  char *thirdcommand[5];
  char *fourthcommand[5];

  int firstIndex = 0, secondIndex = 0, thirdIndex = 0, fourthIndex = 0;

  printf("$ ");
  fgets(inputStr, sizeof(inputStr), stdin);
  inputStr[strcspn(inputStr, "\n")] = 0;
  printf("\n");
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
      processpipeline(firstcommand, NULL, NULL, NULL);
      break;
    case 2:
      processpipeline(firstcommand, secondcommand, NULL, NULL);
      break;
    case 3:
      processpipeline(firstcommand, secondcommand, thirdcommand, NULL);
      break;
    case 4:
      processpipeline(firstcommand, secondcommand, thirdcommand, fourthcommand);
      break;
  }
}

int main()
{
    char *firstcommand[] = {"cat", "file3.txt", NULL};
    char *secondcommand[] = {"grep", "yasin", NULL};
    char *thirdcommand[] = {"tee", "file4.txt", NULL};
    char *fourthcommand[] = {"wc", "-l", NULL};

    readParseInput();

    // processpipeline(firstcommand, secondcommand, NULL, NULL);

    return 0;
}