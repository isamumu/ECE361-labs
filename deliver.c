#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>

#define MAXDATASIZE 4096 //got this number from my ECE344 lab, subject to 
#define MAXBUFLEN 4096
#define PORT 5901

// the execution command should have the following structure: deliver <server address> <server port number>
////////////////////////////////////////////////////////////////////////////////////
//// upon execution the client should:                                          ////
////  1. Ask the user to input a message as follows: ftp <file name>            ////
////  2. Check the existence of the file                                        ////
////      a. if the message is "yes", print out "A file transfer can start"     ////
////      b. else, exit                                                         ////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv){
    
    // Section 1 (see page 35 on Beej's guide for useful tips)
    int sockfd;
    char buf[MAXBUFLEN];
    struct addrinfo *servinfo;
    socklen_t addrlen;
    struct sockaddr_in servaddr;
    char *ftp = "ftp";
    int port = atoi(argv[2]);

    if (argc != 3) { //input format: deliver <server address> <server port number>
        printf("usage: client hostname\n");
        exit(1);
    }

    printf("please enter filename to transfer: ftp <file name>\n");
    char key[10], file[35];
    scanf("%s %s", key, file);

    if(strcmp(ftp, "ftp")){
        perror("invalid input: exiting");
        exit(1);
    }

    if(access(file, F_OK)!=0){
        printf("file DNE");
        exit(1);
    } else{
        printf("file found\n");
    }
    
    //see getaddrinfo
    
    // try opening the socket, print an error otherwise
    if ((sockfd=socket(AF_INET, SOCK_DGRAM, 0))< 0) { 
        perror("failed to create a socket!!"); 
        exit(1); 
    } else{
        printf("socket creation successful!\n");
    } 

    // set server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(port); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 

    // stuff Beej's guide recommends
    printf("client: connecting... \n");

    int sentftp;
     
    if((sentftp = sendto(sockfd, (const char *)ftp, strlen(ftp), 0, (struct sockaddr *) &servaddr, sizeof servaddr)) == -1){
        perror("failed to send ftp");
        exit(1);
    } else{
        printf("message sent successfully\n");
    }
    
    // receive a response from the server
    addrlen = sizeof(servaddr);
    int nbits =  recvfrom(sockfd, buf, MAXBUFLEN-1, 0, (struct sockaddr *)&servaddr, &addrlen);
    
    buf[nbits] = '\0';

    //printf("value of buffer: %s\n", buf);
    if(nbits == -1){
        perror("failed to receive from server");
        exit(1);
    }
    
    if(strcmp(buf, "yes") == 0){
        printf("A file transfer can start\n");
    } else {
        printf("file transfer may not proceed\n");
        exit(1);
    }
    
    close(sockfd);
    return 0;
    
}