#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>

#define BACKLOG 10 //followed the guide, not sure if this is the correct number
#define PORT 5901

int main(int argc, char *argv[]){
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
    
    // getaddrinfo() that does all kinds of goodies for you, including DNS and service name lookups, and fills out the structs you need
    // steps: 1. getaddrinfo 2. socket() for File Descriptor 3. bind to find port 4. connect()

    // serv_addr is a struct sockaddr containing the destination port and IP address
    
    
    struct sockaddr_in servaddr, cliaddr;
    struct addrinfo hints;
    struct addrinfo *servinfo; // will point to the results
    
    int sockfd;
    socklen_t clilen, addr_size;
    
    char* port = argv[2]; // get the port
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 

    // try opening the socket, print an error otherwise
    if ((sockfd=socket(AF_INET, SOCK_DGRAM, 0))< 0) { 
        perror("failed to create a socket!!"); 
        exit(1); 
    } else{
        printf("socket creation successful!\n");
    }
    
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr));
    
    
    // Fill in the server info
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT);
    
    // check for bind error
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof servaddr) == -1) {
        perror("failed to bind!!");
        exit(1);
    } else{
        printf("bind successful!\n");
    }
    
    printf("one moment please...\n");
    
    int BUF_SIZE = 50; // len is the maximum length of the buffer which we don't know

    char buffer[BUF_SIZE]; //= '\0'; // buf is the buffer to read the information into. Gonna store stuff
    clilen = sizeof(cliaddr);

    int received = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cliaddr, &clilen); 
    if(received == -1){
        perror("failed to receive from client");
        exit(1);
    }
    
    buffer[BUF_SIZE] = '\0';

    if(strcmp(buffer, "ftp")){
        int sentYes = sendto(sockfd, "yes", strlen("yes"), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
        if(sentYes == -1){
            perror("failed to send yes");
            exit(1);
        }
            
    } else{
        int sentNo = sendto(sockfd, "no", strlen("no"), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
        if(sentNo == -1){
            perror("failed to send no");
            exit(1);
        }
    }
    
    // close up
    close(sockfd); 
    
    return 0;
}