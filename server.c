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
    struct sockaddr_in cliaddr;
    struct sockaddr_storage client_sock; //client connection
    struct addrinfo *servinfo, hints; 
    char buffer[BUF_SIZE];
    int sockfd;
    int dummy;
    socklen_t clilen, addr_size;
    
    char * port = argv[1]; // get the port
    char* yes = "yes";
    char* no = "no";
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE; // use my I

    // obtain IP address
    if ((dummy = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        perror("cannot get IP address");
        return 1;
    }

    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        perror("server: socket");
    }

    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: bind");
    }
    
    //finished with finding the IP address
    freeaddrinfo(servinfo);
    
    printf("one moment please.....\n");
    clilen = sizeof(struct sockaddr_storage);

    int nbits = recvfrom(sockfd, (char*)buffer, BUF_SIZE, 0, (struct sockaddr *) &client_sock, &clilen); 

    if(nbits == -1){
        perror("failed to receive from client");
        exit(1);
    } else{
        printf("received: #%d\n", nbits);
    }
    
    buffer[nbits] = '\0'; // end indicator
    //printf("valud of buffer: %s\n", buffer);

    if(strcmp(buffer, "ftp") == 0){
        int sentYes = sendto(sockfd, (const char *)yes, strlen(yes), 0, (struct sockaddr *) &client_sock, sizeof(cliaddr));
        if(sentYes == -1){
            perror("failed to send yes");
            exit(1);
        } else{
            printf("sent to client!\n");
        }
            
    } else{
        int sentNo = sendto(sockfd, (const char *)no, strlen(no), 0, (struct sockaddr *) &client_sock, sizeof(cliaddr));
        if(sentNo == -1){
            perror("failed to send no");
            exit(1);
        }
    }
    
    // close up
    close(sockfd); 
    
    return 0;
}