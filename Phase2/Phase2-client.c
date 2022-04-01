#include <unistd.h> 
#include <stdio.h>  
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <arpa/inet.h> 

#define PORT 5000

int main()
{
  int sock = 0;
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
  while(1){
    // variable such as message buffers to receive and send messages
    char inputStr[1000];
    fgets(inputStr, sizeof(inputStr), stdin);
    
    send(sock,inputStr,strlen(inputStr),0); // send hello message to server
  }


  close(sock); // close the socket/end the conection

}

