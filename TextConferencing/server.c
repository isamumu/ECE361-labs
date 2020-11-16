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
#include <stdbool.h>
#include "message.h"

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

//inside each session there is another users list for all the users that joined the session.
struct session *session_list = NULL; //list for all the sessions being created
struct user *user_list = NULL; //list for every users currently logged in
//struct account *user1;
//struct account *user2;
struct account *accounts = NULL;

void message_handler(int sockfd, char *msgRecv) {
    struct message *newMsg = (struct message *)malloc(sizeof(struct message)); 
    struct message *respMsg = (struct message *)malloc(sizeof(struct message));
    newMsg = formatString(msgRecv);
    char buff[BUF_SIZE];
    int numbytes;

    if (newMsg == NULL) {
        printf("NOPE");
	    return;
    }

    // TODO after creating linked list functions, implement 
    // Hannah: session functions (create join and leave)
    // Isamu: rest

    if (newMsg->type == LOGIN) {
        printf("login recieved\n");

        if (newMsg->data == NULL || newMsg->source == NULL || newMsg->size == 0) {
            return;
        }
        char *password = newMsg->data;
        char *id = newMsg->source;
        printf("i'm here\n");
            //bool match = false;
        struct user *newUser = (struct user *)malloc(sizeof(struct user));
        
        //TODO: match and find user and password from list
	    //bool match = true;
        //check for user name and password
        //send back ACK and NACK accordingly
        if(findAcct(accounts, id, password)){
            respMsg->type = LO_ACK;
            strcpy(newUser->name, newMsg->source);
            strcpy(newUser->password, newMsg->data);
            newUser->sessionID = NULL;
            newUser->sockfd = sockfd;
            newUser->next = NULL; // we just add a user here not to a session yet
            newUser->user_cnt = -1;
            addUser(user_list, newUser);
        } else{
            respMsg->type = LO_NACK;
        }

        formatMessage(respMsg, buff);

        if((numbytes = send(sockfd, buff, BUF_SIZE - 1, 0)) == -1){
            fprintf(stderr, "ACK error\n");
            close(sockfd);
            return;
        }
        
        printUsers(user_list);
        free(respMsg);
        free(newMsg);
    }
    else if (newMsg->type == EXIT) {
        printf("Exit recieved\n");
            //exit the server
            //get rid of sockfd
    }
    else if (newMsg->type == JOIN) {
        //find the session in the session list
        //put the sockfd to the sockfdlist
        //assign a session number
        //send back ACK and NACK accordingly
        printf("JOIN recieved\n");
        if (newMsg->source == NULL) {
            return;
        }

        struct session *session_found = findSession(session_list, newMsg->source);
        if (session_found == NULL) {
            return;
        }
        
        struct user *this_user = findUser(user_list, sockfd);
        if (this_user == NULL) {
            return;
        }

        if (this_user->sessionID == NULL) {
            this_user->sessionID = newMsg->source;
            respMsg->type = JN_ACK;
            strncpy(respMsg->data, newMsg->source, MAX_DATA);
            struct user *myuser = (struct user *)malloc(sizeof(struct user));
            myuser->sessionID = this_user->sessionID;
            strncpy(myuser->name, this_user->name, MAX_NAME);
            strncpy(myuser->password, this_user->password, MAX_NAME);
    	    myuser->sockfd = this_user->sockfd;
    	    myuser->user_cnt = -1;
    	    myuser->next = NULL;
            addUser(session_found->users, myuser);
        }
        else {
            respMsg->type = JN_NACK;
        }

        formatMessage(respMsg, buff);

        if ((numbytes = send(sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
            fprintf(stderr, "ACK error\n");
            close(sockfd);
            return;
        }
        printSessions(session_list);
        printUsers(user_list);

    }
    else if (newMsg->type == LEAVE_SESS) {
        //delete the session from the session list
        //send back ACK and NACK
        //if every user leaves the linked list, then we change the name of session back to NULL
	printf("LEAVE_SESS recieved\n");
        struct user *this_user = findUser(user_list, sockfd);
	if (this_user == NULL) {
	   return;
	}
	if (this_user->sessionID != NULL) {
            struct session *session_found = findSession(session_list, this_user->sessionID);
	    printf("session name: %s\n", this_user->sessionID);
	    if (session_found == NULL) {
	    	return;
	    }
	    //this_user->sessionID = NULL;
	    removeUser(session_found->users, this_user);
            if (session_found->users->user_cnt == -1) {
		printf("session name: %s\n", this_user->sessionID);
               	removeSession(session_list, this_user->sessionID);
            }
	}
	this_user->sessionID = NULL;
	printSessions(session_list);
	printUsers(user_list);
    } 
    else if (newMsg->type == NEW_SESS) {
        //check if max seesion number has reached
        //create a new session and put the sockfd into the sockfd list
        //increment the fd count
        //add the new session to the session list
        //assign a session number
        //send back ACK and NACK accordingly
        //struct session *session_found = findSession(session_list, newMsg->data);
        printf("NEW_SESS recieved\n");
        struct user *this_user = findUser(user_list, sockfd);
        
        if (this_user == NULL) {
            printf("user not found\n");
            return;
        }

        struct session *newSession = (struct session *)malloc(sizeof(struct session));
        struct user *dummy = (struct user *)malloc(sizeof(struct user));
        struct user *myuser = (struct user *)malloc(sizeof(struct user));

        strncpy(myuser->name, this_user->name, MAX_NAME);
        strncpy(myuser->password, this_user->password, MAX_NAME);

    	myuser->sockfd = this_user->sockfd;
    	myuser->user_cnt = -1;
    	myuser->next = NULL;
        dummy->user_cnt = 0;
        dummy->sessionID = NULL;
        dummy->sockfd = -1;
    	//dummy->next = myuser;
        newSession->sessionName = newMsg->source;
        newSession->users = dummy;
	    addUser(newSession->users, myuser);
        //newSession->users->user_cnt = 1;
        newSession->next = NULL;

        if (this_user->sessionID == NULL) {
            this_user->sessionID = newMsg->source;
            myuser->sessionID = newMsg->source;
            addSession(session_list, newSession);
            respMsg->type = NS_ACK;
        }
        
        formatMessage(respMsg, buff);

        if ((numbytes = send(sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
            fprintf(stderr, "ACK error\n");
            close(sockfd);
            return;
        }
        
        printUsers(user_list);
        printSessions(session_list);
    }
    else if (newMsg->type == MESSAGE) {
        //check the session that the sender is in
        //send the message to everyone (every sockfd) in the sockfdlist of that session
	    printf("MESSAGE recieved\n");
    }

    else { // newMsg.type == QUERY
        //print out the list of user and avaliable sessions
        //send back ACK
	    printf("QUERY recieved\n");
        respMsg->type = QU_ACK;
        //struct session *ptr = (struct session *)malloc(sizeof(struct session));
        //ptr = session_list->next;
        //trcpy(respMsg->data, "hello there");
        //respMsg->size = strlen(respMsg->data);

        char* data = "";
        struct user *ptr = user_list->next;
        while (ptr != NULL) {
            strcat(data, ptr->name);
            printf("data1: %s\n", data);
            strcat(data, "->");
            printf("data2: %s\n", data);
            strcat(data, ptr->sessionID);
            printf("data3: %s\n", data);
            strcat(data, "\n");
            ptr = ptr->next;
        }
        printf("data: %s", data);
        strncpy(respMsg->source, data, MAX_DATA);
        printf("source: %s\n", respMsg->source);
    
        /*for(int i = 0; i < session_list->session_cnt; i++){
            strcat(respMsg->source, ptr->sessionName);
            strcat(respMsg->source, ": \n");
            struct user *uptr = ptr->users->next;

            while(1){
                
                strcat(respMsg->source, uptr->name);
                strcat(respMsg->source, "\n");

                if(uptr->next == NULL){
                    break;
                }
                uptr = uptr->next;
            }

            ptr = ptr->next;

        }*/
        
        
        memset(buff, 0, BUF_SIZE);
        formatMessage(respMsg, buff);
        //printf("the string is here %s", buff);

        if((numbytes = send(sockfd, buff, BUF_SIZE - 1, 0)) == -1){
            fprintf(stderr, "ACK error\n");
            close(sockfd);
            return;
        }
    }
    return;
}

void list_init(void) {
    struct session *mysession = (struct session *)malloc(sizeof(struct session));
    mysession->session_cnt = 0;
    mysession->users = NULL;
    mysession->next = NULL;
    mysession->sessionName = NULL;
    session_list = mysession;

    struct user *myuser = (struct user *)malloc(sizeof(struct user));
    myuser->sessionID = NULL;
    myuser->sockfd = -1;
    myuser->user_cnt = 0;
    myuser->next = NULL;
    user_list = myuser;
    return;
}

int main(int argc, char *argv[]){
    
    //TODO: create dictionary of user and password
    // Isamu

    //inside each session there is another users list for all the users that joined the session.
    //struct session *session_list; //list for all the sessions being created
    //struct user *user_list; //list for every users currently logged in

    // initialize users
    struct account *user1 = (struct account *)malloc(sizeof(struct account));
    struct account *user2 = (struct account *)malloc(sizeof(struct account));
    strcpy(user1->id, "isamu");
    strcpy(user1->password, "ECE2T1");
    user1->next = user2;

    strcpy(user2->id, "hannah");
    strcpy(user2->password, "ECE2T2");
    user2->next = NULL;
    accounts = user1;

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
    list_init();
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    
    char * port = argv[1]; // get the port 
     
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE; // use my I
 
    // obtain IP address 
    if ((numbytes = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        perror("cannot get IP address");
        return 1;
    }

    struct addrinfo *p;
    for (p = servinfo; p != NULL; p = p->ai_next) {
    	if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
	        continue;
    	}
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    	if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
	        continue;
    	}

	break;
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
        printf("looping.");
        memset(buffer, 0, BUF_SIZE);
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1 ) {
            perror("select");
            exit(1);
        }

        for (connection = 0; connection <= fdmax; connection++) {
	        
            printf("connection: %d\n", connection);
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
                        printf("new connection made on socket %d current fdmax: %d\n", userfd, fdmax);
                    }

                }
                else {
		            printf("message recieved from socket %d\n", connection);
                    if ((numbytes = recv(connection, buffer, BUF_SIZE, 0)) == -1) {
                        perror("ERROR: recv");
                        close(connection);
                        FD_CLR(connection, &master);
                    }
                    if (numbytes <= 3) {
                        printf("ignore socket %d\n", connection);
                        //close(connection);
                        //FD_CLR(connection, &master);
                    }
                    else {
			            printf("going through message_handler: %s\n", buffer);
                        message_handler(connection, buffer);
                    }
                }
            }
        }
    }
}
