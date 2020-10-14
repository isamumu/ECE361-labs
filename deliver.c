#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

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

int main(int argc, char **argv){
    //===================Section 1=====================================================
    // (see page 35 on Beej's guide for useful tips)

    int sentftp;
    int sockfd;
    char buf[MAXBUFLEN];
    struct addrinfo *servinfo, hints;
    socklen_t addrlen;
    struct sockaddr_storage server_sock; // connector address info
    int dummy;
    char *ftp = "ftp";
    char * port = argv[2];

    clock_t start, end;
    double cpu_time_used;

    if (argc != 3) { //input format: deliver <server address> <server port number>
        printf("usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    // find the IP at the specified port (FIND THE SERVER)
    if ((dummy = getaddrinfo(argv[1], port, &hints, &servinfo)) != 0) {
        perror("bad retrieval");
        return 1;
    }

    // get socket from server port
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        perror("server: socket");
    }

    printf("please enter filename to transfer: ftp <file name>\n");
    char key[10], file[35];
    scanf("%s %s", key, file);

    if(strcmp(key, "ftp")){
        perror("invalid input: exiting");
        exit(1);
    }

    if(access(file, F_OK)!=0){
        printf("file DNE\n");
        exit(1);
    } else{
        printf("file found\n");
    }
    

    // stuff Beej's guide recommends
    printf("client: connecting... \n");
     
    start = clock(); // start measuring RTT
    if((sentftp = sendto(sockfd, (const char *)ftp, strlen(ftp), 0, servinfo->ai_addr, servinfo->ai_addrlen)) == -1){
        perror("failed to send ftp");
        exit(1);
    } else{
        printf("message sent successfully\n");
        // sent to the server. Now we wait for a response back to complete the round trip
    }
    
    
    // receive a response from the server
    addrlen = sizeof(struct sockaddr_storage);
    int nbits =  recvfrom(sockfd, buf, MAXBUFLEN-1, 0, (struct sockaddr *)&server_sock, &addrlen);
    
    buf[nbits] = '\0';

    //printf("value of buffer: %s\n", buf);
    if(nbits == -1){
        perror("failed to receive from server");
        exit(1);
    }
    printf("nbits: %d\n", nbits);
    if(strcmp(buf, "yes") == 0){
        printf("A file transfer can start\n");
        end = clock();
    } else {
        printf("file transfer may not proceed\n");
        exit(1);
    }

    RTT = ((double) (end - start)) / CLOCKS_PER_SEC; // calculate RTT in seconds
    print("RTT = %d", RTT);
    
    close(sockfd);
    return 0;

    //===================Section 2&3=====================================================
    // Section 2: How long is the measured round trip time?
    
}
