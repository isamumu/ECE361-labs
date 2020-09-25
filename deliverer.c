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
    char *ftp_msg = "ftp";

    if (argc != 3) { //input format: deliver <server address> <server port number>
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) { //the second argument is the IP addr, third is the port #
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // the below also obtains the socket information. With that we can send some info
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        perror("client: socket");
        exit(1);
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        perror("client: connect");
        exit(1);
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    // stuff Beej's guide recommends
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';
    printf("client: received '%s'\n",buf);
    close(sockfd);
    return 0;

    // send the ftp message
    int sentftp = sendto(sockfd, ftp_msg, strlen(ftp_msg), 0, (struct sockaddr *) &servinfo, sizeof(servinfo));
    if(sentftp == -1){
        perror("failed to send ftp");
        exit(1);
    }
    char buffer[MAXBUFLEN - 1]; //= 0; //not sure what you means by intializing this to 0
    // buf is the buffer to read the information into. Gonna store stuff
    addr_len = sizeof their_addr;

    // receive a response from the server
    int received =  recvfrom(sockfd, buffer, MAXBUFLEN-1, 0, (struct sockaddr *)&their_addr, &addr_len);
    if(received == -1){
        perror("failed to receive from server");
        exit(1);
    }

    if(strcmp(buffer, "yes")){
        char *success_msg = "A file transfer can start";
        int sentMsg = sendto(sockfd, ftp_msg, strlen(ftp_msg), 0, (struct sockaddr *) &servinfo, sizeof(servinfo));
    } else {
        exit(1);
    }
    
    close(sockfd);
    return 0;
}