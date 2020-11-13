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
#include <message.h>
#include "packet.h"
#include "message.h"

#define BACKLOG 10
#define SESSION_SIZE 10
#define USER_SIZE 10

char *sessionList[10];
struct user userlist[10];
int userCount = 0;

void message_handler(int sockfd, char *msgRecv) {
    struct message *newMsg;
    newMsg = formatString(msgRecv);
    if (newMsg.type == LOGIN) {
        //check for user name and password
        //send back ACK and NACK accordingly
    }
    else if (newMsg.type == EXIT) {
        //exit the server
    }
    else if (newMsg.type == JOIN) {
        //check if max seesion number has reached
        //create a new session and put the sockfd into the sockfd list
        //increment the fd count
        //add the new session to the session list
        //send back ACK and NACK accordingly
    }
    else if (newMsg.type == LEAVE_SESS) {
        //
    }
}

int main(int argc, char *argv[]){
    

    //===================Section 1=====================================================
    struct sockaddr_in cliaddr;
    struct sockaddr_storage client_sock; //client connection
    struct addrinfo *servinfo, hints; 
    char buffer[BUF_SIZE];
    int sockfd, userfd;
    int numbytes; 
    socklen_t clilen, addr_size;
    fd_set master;
    fd_set read_fds;
    int fdmax;

    int connection, other_connection;

    int yes = 1;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    
    char * port = argv[1]; // get the port 
     
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE; // use my I
 
    // obtain IP address 
    if ((numbytes = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        perror("cannot get IP address");
        return 1;
    }
 
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        perror("server: socket");
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: bind");
    }
    
    //finished with finding the IP address
    freeaddrinfo(servinfo);

    if ((numbytes = listen(sockfd, BACKLOG)) == -1) {
        perror("server: listen");
        exit(1);
    }

    FD_SET(sockfd, &master);

    fdmax = sockfd;
    
    printf("one moment please.....\n");

    while(1) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1 ) {
            perror("select");
            exit(1);
        }

        for (connection = 0; connection <= fdmax; connection++) {
            if (FD_ISSET(connection, &read_fds)) {
                if (connection == sockfd) {
                    //new connection recieved
                    addr_size = sizeof(client_sock);
                    if ((userfd = accept(sockfd, (struct sockaddr *)&client_sock, &addr_size)) == -1) {
                        perror("ERROR: accept");
                    }
                    else {
                        FD_SET(userfd, &master);
                        if (userfd > fdmax) {
                            fdmax = userfd;
                        }
                        printf("new connection made on aocket %d\n", userfd);
                    }

                }
                else {
                    if ((numbytes = recv(connection, buffer, BUF_SIZE, 0)) == -1) {
                        perror("ERROR: recv");
                        close(connection);
                        FD_CLR(connection, &master);
                    }
                    if (numbytes == 0) {
                        printf("socket %d closed\n", connection);
                        close(connection);
                        FD_CLR(connection, &master);
                    }
                    else {
                        message_handler(connection, buffer);
                    }
                }
            }
        }
    }
}


























