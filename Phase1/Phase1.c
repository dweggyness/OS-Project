#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void processpipeline(char *firstcommand[], char *secondcommand[] , char *thirdcommand[], char *fourthcommand[])
{
    int fd1[2]; // pipe 1 for getting output of ls from child 1 and giving it to child 2 also
    int fd2[2]; // pipe 2 for getting output of grep from child 2 and giving it to parent wc
    int fd3[2]; // pipe 3 for getting output from cmd 3, and giving it to cmd 4
    int pid;
  
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
              printf("THIRD CHILD PT 1");
              close(fd1[1]);
              close(fd1[0]);
              close(fd2[1]);
              close(fd2[0]);
              close(fd3[0]);
              close(fd3[1]);
              printf("THIRD CHILD PT 2");
              execvp(thirdcommand[0], thirdcommand);
              printf("THIRD CHILD FINISH");
              perror ("Execvp failed while executing grep");
              exit(EXIT_FAILURE);

            }
            else { // parent for cmd 4
              printf("%d: parent - wc\n", getpid());
              if (fourthcommand == NULL) {
                printf("return \n");
                exit(0);
              }
              printf("RUNNING CMD 4");
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

int main()
{

    char *firstcommand[] = {"cat", "file3.txt", NULL};
    char *secondcommand[] = {"grep", "yasin", NULL};
    char *thirdcommand[] = {"tee", "file4.txt", NULL};
    // char *fourthcommand[] = {"wc", "-l", NULL};
    processpipeline(firstcommand, secondcommand, thirdcommand, NULL);
    return 0;
}