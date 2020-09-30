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

void *get_in_addr(struct sockaddr *socket_addr) {
    if (socket_addr->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)socket_addr)->sin_addr);
    }

    return &(((struct sockaddr_in6*)socket_addr)->sin6_addr);
}

int main(int argc, char **argv){
    // the execution command should have the following structure: deliver <server address> <server port number>
    ////////////////////////////////////////////////////////////////////////////////////
    //// upon execution the client should:                                          ////
    ////  1. Ask the user to input a message as follows: ftp <file name>            ////
    ////  2. Check the existence of the file                                        ////
    ////      a. if the message is "yes", print out "A file transfer can start"     ////
    ////      b. else, exit                                                         ////
    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////

    // Section 1 (see page 35 on Beej's guide for useful tips)
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints;
    struct addrinfo *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    int rv;
    char s[INET6_ADDRSTRLEN];
    struct sockaddr_in servaddr;

    if (argc != 3) { //input format: deliver <server address> <server port number>
        printf("usage: client hostname\n");
        exit(1);
    }

    char *ip_addrress = argv[2];
    //int port = atoi(argv[3]);

    printf("please enter filename to transfer: ftp <file name>\n");
    char ftp[10], file[35];
    scanf("%s %s", ftp, file);

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

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    
    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) { //the second argument is the IP addr, third is the port #
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
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
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 

    // stuff Beej's guide recommends
    printf("client: connecting... \n");
    freeaddrinfo(servinfo); // all done with this structure

    int sentftp = sendto(sockfd, "ftp", strlen("ftp"), 0, (struct sockaddr *) &servaddr, sizeof servaddr);
    if(sentftp == -1){
        perror("failed to send ftp");
        exit(1);
    } else{
        printf("message sent successfully\n");
    }
    
    // receive a response from the server
    char buffer[MAXBUFLEN - 1]; //= 0; //not sure what you means by intializing this to 0
    int received =  recvfrom(sockfd, buffer, MAXBUFLEN-1, 0, (struct sockaddr *)&their_addr, &addr_len);
    if(received == -1){
        perror("failed to receive from server");
        exit(1);
    }

    buf[received] = '\0';
    printf("client: received '%s'\n",buf);
    // send the ftp message
    
    // buf is the buffer to read the information into. Gonna store stuff
    addr_len = sizeof their_addr;
    
    if(strcmp(buffer, "yes")){
        char *success_msg = "A file transfer can start";
        int sentMsg = sendto(sockfd, "ftp", strlen("ftp"), 0, (struct sockaddr *) &servinfo, sizeof(servinfo));
    } else {
        exit(1);
    }
    
    close(sockfd);
    return 0;
    
}