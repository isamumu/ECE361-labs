//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <message.h>

#define BACKLOG 10
#define SESSION_SIZE 10
#define USER_SIZE 10

// 1. create linked list of dummy users (5 sessions, 5 users per session)
// 2. implement functions to add and remove users from the linked list
// 3. implement function to scan through the entire list
// 4. create dictionary to store predefined users and password pairs

//TODO: create 5 dummy session linked list (with only head) with NULL sessionID
//      create 5 dummy user linked list (with only head) with NULL sessionID
// Isamu

//TODO: create dictionary of user and password
// Isamu

//inside each session there is another users list for all the users that joined the session.
struct session *session_list; //list for all the sessions being created
struct user *user_list; //list for every users currently logged in

void message_handler(int sockfd, char *msgRecv) {
    struct message *newMsg, *respMsg;
    newMsg = formatString(msgRecv);
    char *buff[BUF_SIZE];
    int numbytes;


    // TODO after creating linked list functions, implement 
    // Hannah: session functions (create join and leave)
    // Isamu: rest
    if (newMsg.type == LOGIN) {
        char *password = newMsg->data;
        char *id = newMsg->source;
        bool match = false;
        
        //TODO: match and find user and password from list

        //check for user name and password
        //send back ACK and NACK accordingly
        if(match){
            respMsg.type = LO_ACK;
        } else{
            respMsg.type = LO_NACK;
        }

        formatMessage(respMsg, buff);

        if((numbytes = send(*sockfd, buff, BUF_SIZE - 1, 0)) == -1){
            fprintf(stderr, "ACK error\n");
            close(sockfd);
            return;
        }
    }
    else if (newMsg.type == EXIT) {
        //exit the server
        //get rid of sockfd
    }
    else if (newMsg.type == JOIN) {
        //find the session in the session list
        //put the sockfd to the sockfdlist
        //assign a session number
        //send back ACK and NACK accordingly
    }
    else if (newMsg.type == LEAVE_SESS) {
        //delete the session from the session list
        //send back ACK and NACK
        //if every user leaves the linked list, then we change the name of session back to NULL
    } 
    else if (newMsg.type == NEW_SESS) {
        //check if max seesion number has reached
        //create a new session and put the sockfd into the sockfd list
        //increment the fd count
        //add the new session to the session list
        //assign a session number
        //send back ACK and NACK accordingly
    }
    else if (newMsg.type == MESSAGE) {
        //check the session that the sender is in
        //send the message to everyone (every sockfd) in the sockfdlist of that session
    }
    else { // newMsg.type == QUERY
        //print out the list of user and avaliable sessions
        //send back ACK
        respMsg.type = QU_ACK;
        formatMessage(respMsg, buff);

        if((numbytes = send(*sockfd, buff, BUF_SIZE - 1, 0)) == -1){
            fprintf(stderr, "ACK error\n");
            close(sockfd);
            return;
        }
    }
    return;
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
                        printf("new connection made on socket %d\n", userfd);
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
















