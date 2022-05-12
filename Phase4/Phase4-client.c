#include <unistd.h> 
#include <stdio.h>  
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <arpa/inet.h> 
#include <signal.h> // header for signal related functions and macros declarations

#define PORT 5000

int sock = 0;
// function routine of Signal Handler for SIGINT, to send connection termination message to server and terminates the client process
void clientExitHandler(int sig_num)
{
  send(sock,"exit_client",strlen("exit"),0); // sending exit message to server
  close(sock); // close the socket/end the conection
  printf("Exiting client.  \n");
  fflush(stdout);// force to flush any data in buffers to the file descriptor of standard output,, a pretty convinent function
  exit(0);
}

int main()
{
  signal(SIGINT, clientExitHandler);
  sock = 0;
  struct sockaddr_in serv_addr; // structure for storing addres; remote server ip and port 
  
  // Creating socket file descriptor with communication: domain of internet protocol version 4, type of SOCK_STREAM for reliable/conneciton oriented communication, protocol of internet 
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) // checking if socket creation fail
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  // setting the address to connect socket to server
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  /*
  Convert IPv4 and IPv6 addresses from text to binary form and set the ip
  This function converts the character string 127.0.0.1 into a network
  address structure in the af address family, then copies the
  network address structure to serv_addr.sin_addr
  */

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) // check if conversion failed
  {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  // connect the socket with the adddress and establish connnection with the server 
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\nConnection Failed \n");
    return -1;
  }
  char inputStr[4096];

  printf("Successfully connected to server at port %d! \n", PORT);
  
  // receive initial message from server
  char message[4096] = {0};
  recv(sock, message, sizeof(message),0); 
  printf("%s", message);

  while(1){
    // variable such as message buffers to receive and send messages 
    
    printf("@ ");
    fgets(inputStr, sizeof(inputStr), stdin);
    inputStr[strcspn(inputStr, "\n")] = 0;
    if (strlen(inputStr) == 0) {
      continue;
    }
   
    // handle exit from client side
    if (strcmp(inputStr, "exit") == 0) {
      printf("Closing socket from client side \n");
      send(sock,inputStr,strlen(inputStr),0); // send exit message to server
      break;
    }

    send(sock,inputStr,strlen(inputStr),0); // send input message to server

    // after user input, wait for server response and print server response
    char response[4096] = {0};
    recv(sock, response, sizeof(response),0); // receive input string from server

    printf("%s \n", response);

  }
  printf("Exited \n");


  close(sock); // close the socket/end the conection
}

