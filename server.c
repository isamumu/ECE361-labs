#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>


int main(){
    // the execution command should have the following structure: server <UDP listen port>
    ////////////////////////////////////////////////////////////////////////////////////
    //// upon execution the server should:                                          ////
    ////  1. Open a UDP socket and listen at the specified port number              ////
    ////  2. Receive a message from the client                                      ////
    ////      a. if the message is "ftp", reply with a message "yes" to the client. ////
    ////      b. else, reply with a message "no" to the client                      ////
    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////

    //===================Section 1=====================================================
    
    int sockfd; 
    char command[100];
    char port_number[16];
    struct sockaddr_in servaddr;

    printf("Enter command: ");
    scanf("%s", command);
   

    // try opening the socket, print an error otherwise
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("failed to create a socket!!"); 
        exit(1); 
    }

    //?
    memset(&servaddr, 0, sizeof(servaddr)); 

    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(port_number); 

    //?
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("failed to bind the socket!!"); 
        exit(1); 
    } 
      
    
}