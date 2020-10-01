#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>

#define BUF_SIZE 1024
#define PORT 5901

// the execution command should have the following structure: server <UDP listen port>
////////////////////////////////////////////////////////////////////////////////////
//// upon execution the server should:                                          ////
////  1. Open a UDP socket and listen at the specified port number              ////
////  2. Receive a message from the client                                      ////
////      a. if the message is "ftp", reply with a message "yes" to the client. ////
////      b. else, reply with a message "no" to the client                      ////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]){
    

    //===================Section 1=====================================================
    
    struct sockaddr_in servaddr, cliaddr;
    
    char buffer[BUF_SIZE];
    int sockfd;
    socklen_t clilen, addr_size;
    
    int port = atoi(argv[1]); // get the port
    char* yes = "yes";
    char* no = "no";
    
    // try opening the socket, print an error otherwise
    if ((sockfd=socket(AF_INET, SOCK_DGRAM, 0))< 0) { 
        perror("failed to create a socket!!"); 
        exit(1); 
    } else{
        printf("socket creation successful: #%d\n", sockfd);
    }
    
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr));
    
    // Fill in the server info
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(port);
    
    // check for bind error
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof servaddr) == -1) {
        perror("failed to bind!!");
        exit(1);
    } 
    
    printf("one moment please...\n");
    clilen = sizeof(cliaddr);

    int nbits = recvfrom(sockfd, (char*)buffer, BUF_SIZE, 0, (struct sockaddr *) &cliaddr, &clilen); 

    if(nbits == -1){
        perror("failed to receive from client");
        exit(1);
    } else{
        printf("received: #%d\n", nbits);
    }
    
    buffer[nbits] = '\0'; // end indicator
    //printf("valud of buffer: %s\n", buffer);

    if(strcmp(buffer, "ftp") == 0){
        int sentYes = sendto(sockfd, (const char *)yes, strlen(yes), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
        if(sentYes == -1){
            perror("failed to send yes");
            exit(1);
        } else{
            printf("sent to client!\n");
        }
            
    } else{
        int sentNo = sendto(sockfd, (const char *)no, strlen(no), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
        if(sentNo == -1){
            perror("failed to send no");
            exit(1);
        }
    }
    
    // close up
    close(sockfd); 
    
    return 0;
}