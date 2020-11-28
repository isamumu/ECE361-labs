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
#include <pthread.h>

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
int userCount = 0;
int sessionCount = 0;

pthread_mutex_t sessions_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t users_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t scnt_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ucnt_lock = PTHREAD_MUTEX_INITIALIZER;

//void message_handler(int sockfd, char *msgRecv, int *exited) {
void *message_handler(void *arg) {
    printf("new thread created\n");
    struct user *newUser = (struct user *)arg;
    printf("sockfd: %d\n", newUser->sockfd);
    struct message *newMsg;// = (struct message *)malloc(sizeof(struct message)); 
    struct message *respMsg = (struct message *)malloc(sizeof(struct message));
    //newMsg = formatString(msgRecv);
    char buff[BUF_SIZE];
    int numbytes;
    bool exited = false;
    bool isLogin = false;


    // TODO after creating linked list functions, implement 
    // Hannah: session functions (create join and leave)
    // Isamu: rest

    while (1) {
	memset(buff, 0, BUF_SIZE);
	if ((numbytes = recv(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
	    perror("ERROR: recv\n");
	    exit(1);
	}
        if (numbytes == 0) {
	    exited = true;
	}

	//printf("Message recieved: %s\n", buff);
	newMsg = formatString(buff);

	if (newMsg->type == EXIT) {
	    printf("Exit recieved\n");
	    exited = true;
	}
        if (!isLogin) {
            if (newMsg->type == LOGIN) {
                printf("login recieved\n");

                //if (newMsg->data == NULL || newMsg->source == NULL || newMsg->size == 0) {
                    //return;
                //}
                char *password = newMsg->data;
                char *id = newMsg->source;

                
        
                //TODO: match and find user and password from list
                //check for user name and password
                //send back ACK and NACK accordingly
                if(findAcct(accounts, id, password)){
                    respMsg->type = LO_ACK;
		    struct user *myuser = (struct user *)malloc(sizeof(struct user));
                    strcpy(newUser->name, newMsg->source);
                    strcpy(newUser->password, newMsg->data);
                    newUser->sessionID = NULL;
                    //newUser->sockfd = sockfd;
                    newUser->next = NULL; // we just add a user here not to a session yet
		    memcpy(myuser, newUser, sizeof(struct user));
                    //newUser->user_cnt = -1;
	            pthread_mutex_lock(&users_lock);
                    addUser(&user_list, newUser);
	            pthread_mutex_unlock(&users_lock);
		    isLogin = true;
	            printf("user %s logged in\n", newUser->name);
		    //printUsers(user_list);
                } 
	        else {
                    respMsg->type = LO_NACK;
                }

                formatMessage(respMsg, buff);
		printf("respMsg: %s\n", buff);
		//printUser(newUser);
		//printf("i'm here\n");
		//printf("socket: %d\n", newUser->sockfd);
		//printf("uhmm\n");

                if((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1){
                    fprintf(stderr, "ACK error\n");
                }
        
                printUsers(user_list);
                //free(respMsg);
                //free(newMsg);
            }
	    else {
		respMsg->type = LO_NACK;
		exited = true;
		formatMessage(respMsg, buff);
		if ((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
		    perror("ERROR: send\n");
		    exit(1);
		}
	    }
        }
        else if (newMsg->type == EXIT) {
            printf("Exit recieved for user: %s\n", newUser->name);
	    /*removeUser_fd(user_list, sockfd);
	    struct session *ptr = session_list;
	    while (ptr != NULL) {
	        struct user *uptr = ptr->users;
	        while (uptr != NULL) {
		    removeUser_fd(uptr, newUser->sockfd);
	        }
	        ptr = ptr->next;
	    }
	    //close(sockfd);*/
	    exited = true;
	    //printf("socket %d exitted\n", newUser->sockfd);
                //exit the server
                //get rid of sockfd
        }
        else if (newMsg->type == JOIN) {
            //find the session in the session list
            //put the sockfd to the sockfdlist
            //assign a session number
            //send back ACK and NACK accordingly
            printf("JOIN recieved for user: %s\n", newUser->name);
            /*if (newMsg->source == NULL) {
                return;
            }*/

            struct session *session_found = findSession(session_list, newMsg->data);
            if (session_found == NULL) {
                respMsg->type = JN_NACK;
                strcpy(respMsg->source, newMsg->data);
                strcpy(respMsg->data, "session not find");
            }
	        else {
                //struct user *this_user = findUser(user_list, newUser->sockfd);
                //if (this_user == NULL) {
                //respMsg->type = JN_NACK;
		        //strcpy(respMsg->data, "user not found");
                //}

                newUser->sessionID = newMsg->data;
                respMsg->type = JN_ACK;
                strncpy(respMsg->data, newMsg->data, MAX_DATA);
                struct user *myuser = (struct user *)malloc(sizeof(struct user));
                //newUser->sessionID = this_user->sessionID;
                //strncpy(myuser->name, this_user->name, MAX_NAME);
                //strncpy(myuser->password, this_user->password, MAX_NAME);
                //myuser->sockfd = this_user->sockfd;
                //myuser->user_cnt = -1;
                //myuser->next = NULL;
                memcpy((void *)myuser, (void *)newUser, sizeof(struct user));
                pthread_mutex_lock(&sessions_lock);
                addUser(&session_found->users, myuser);
                pthread_mutex_unlock(&sessions_lock);
                printf("user: %s joined session %s\n", newUser->name, newUser->sessionID);

                
	        }
            formatMessage(respMsg, buff);

            if ((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
                fprintf(stderr, "ACK error\n");
            }

            printSessions(session_list);
            printUsers(user_list);
        }
        else if (newMsg->type == LEAVE_SESS) {
            //delete the session from the session list
            //send back ACK and NACK
            //if every user leaves the linked list, then we change the name of session back to NULL
	    printf("LEAVE_SESS recieved for user: %s\n", newUser->name);
            //struct user *this_user = findUser(user_list, newUser->sockfd);
	    //if (this_user == NULL) {
	       //respMsg->type = ;
	    //}
	    if (newUser->sessionID != NULL) {
                struct session *session_found = findSession(session_list, newUser->sessionID);
	        printf("session name: %s\n", newUser->sessionID);
	        //if (session_found != NULL) {
	    	   // return;
	        //}
	        //this_user->sessionID = NULL;
		pthread_mutex_lock(&sessions_lock);
	        removeSessUser(session_found, newUser, &session_list);
		pthread_mutex_unlock(&sessions_lock);
                    //if (session_found->users->user_cnt == -1) {
		        //printf("session name: %s\n", this_user->sessionID);
               	        //removeSession(session_list, this_user->sessionID);
                    //}
		//}
	    }
	    newUser->sessionID = NULL;
	    printSessions(session_list);
	    printUsers(user_list);
        } 
        else if (newMsg->type == NEW_SESS) {
            printf("NEW_SESS recieved for user: %s\n", newUser->name);
           
            if (newUser->sessionID == NULL) {
	        struct session *newSession = (struct session *)malloc(sizeof(struct session));
            print_message(newMsg);
            newUser->sessionID = (char *)malloc(strlen(newMsg->data) + 1);
	        strcpy(newUser->sessionID, newMsg->data);
	        struct user *myuser = (struct user *)malloc(sizeof(struct user));
		    memcpy((void *)myuser, (void *)newUser, sizeof(struct user));
	        newSession->sessionName = newMsg->data;
	        newSession->users = NULL;
	        pthread_mutex_lock(&sessions_lock);
	        addUser(&newSession->users, myuser);
		    pthread_mutex_unlock(&sessions_lock);
	        newSession->next = NULL;
                //newUser->sessionID = newMsg->data;
	        //this_user->sessionID = newMsg->source;
                //myuser->sessionID = newMsg->data;
	        //myuser->sessionID = newMsg->source;
	        pthread_mutex_lock(&scnt_lock);
	        sessionCount++;
	        pthread_mutex_unlock(&scnt_lock);
	        pthread_mutex_lock(&sessions_lock);
                addSession(&session_list, newSession);
	        pthread_mutex_unlock(&sessions_lock);
            	printSessions(session_list); //check
                respMsg->type = NS_ACK;
	        strcpy(respMsg->data, newUser->sessionID);
            }
        
            formatMessage(respMsg, buff);

            if ((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
                fprintf(stderr, "ACK error\n");
            }
        
            printUsers(user_list);
            printSessions(session_list);
        }
        else if (newMsg->type == MESSAGE) {
            //check the session that the sender is in
            //send the message to everyone (every sockfd) in the sockfdlist of that session
	        printf("MESSAGE recieved for user: %s\n", newUser->name);
            //struct user *ptr = user_list;
            
            char *targetSession = strtok(newMsg->data, ",");
            printf("session: %s\n", targetSession);
            char *text = strtok(NULL, ",");
            printf("text: %s\n", text);

            struct user *myUser;
            respMsg->type = MESSAGE;
            strcpy(respMsg->data, text);
            respMsg->size = strlen(respMsg->data);
            //formatMessage(respMsg, buff);
            //struct session *session_found = findSession(session_list, newUser->sessionID);
            if (newUser->sessionID == NULL) {
                memset(respMsg->data, 0, MAX_DATA);
                char tmp[100] = "no Session Joined";
                strcpy(respMsg->data, tmp);
                respMsg->size = strlen(respMsg->data);
                formatMessage(respMsg, buff);
                printf("message sent: %s\n", respMsg->data);
                printf("user not joined in session\n");

                if ((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
                    perror("ERROR: send\n");
                    exit(1);
                }
            }
            else {
                 
                // locate the session and its users
                struct session *session_found = findSession(session_list, targetSession);
                formatMessage(respMsg, buff);
                struct user *ptr = session_found->users;
                printf("message: %s\n", respMsg->data);

                // send to every user in the session
                while(ptr != NULL) {
                    printf("sending message to user %s\n", ptr->name);
                    if ((numbytes = send(ptr->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
                        perror("ERROR: send\n");
                        exit(1);
                    }
                    ptr = ptr->next;
                }
            }
        }

        else if (newMsg->type == INVITE) {
            printf("incoming invitation from %s", newMsg->data);
        }
        else if (newMsg->type == QUERY) {
            //print out the list of user and avaliable sessions
            //send back ACK
	    printf("QUERY recieved for user: %s\n", newUser->name);
            respMsg->type = QU_ACK;
            struct user *ptr = user_list;
            //ptr = session_list->next;
            //trcpy(respMsg->data, "hello there");
            //respMsg->size = strlen(respMsg->data);
            
            char data[MAX_DATA] = "";
            //ptr = user_list->next;
            while (ptr != NULL) {
                if (ptr->sessionID == NULL) {
		    ptr = ptr->next;
		    continue;
	        }
                strcat(data, ptr->name);
                //printf("data1: %s\n", data);
                strcat(data, "->");
                //printf("data2: %s\n", data);
                strcat(data, ptr->sessionID);
                //printf("data3: %s\n", data);
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

            if((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1){
                fprintf(stderr, "ACK error\n");
            }
        }
	if (exited) {
            break;
	}
	//printf("end of loop\n");
	//printUser(newUser);
    }
    printf("socket exit??\n");
    close(newUser->sockfd);
    if (isLogin) {
	pthread_mutex_lock(&users_lock);
	removeUser(&user_list, newUser);
	pthread_mutex_unlock(&users_lock);
	struct session *ptr = session_list;
	while (ptr != NULL) {
	    struct user *uptr = ptr->users;
	    while (uptr != NULL) {
		pthread_mutex_lock(&sessions_lock);
		removeSessUser(ptr, newUser, &session_list);
		pthread_mutex_unlock(&sessions_lock);
	    }
	    ptr = ptr->next;
	}

	free(newUser);
	pthread_mutex_lock(&ucnt_lock);
	userCount--;
	pthread_mutex_unlock(&ucnt_lock);
    }
    else {
        printf("Please login\n");
    }
    return NULL;
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
    struct account *user3 = (struct account *)malloc(sizeof(struct account));
    strcpy(user1->id, "isamu");
    strcpy(user1->password, "ECE2T1");
    user1->next = user2;

    strcpy(user2->id, "hannah");
    strcpy(user2->password, "ECE2T2");
    user2->next = user3;

    strcpy(user3->id, "a");
    strcpy(user3->password, "b");
    user3->next = NULL;
    accounts = user1;

    //===================Section 1=====================================================
    struct sockaddr_in cliaddr;
    struct sockaddr_storage client_sock; //client connection
    struct addrinfo *servinfo, hints; 
    char buffer[BUF_SIZE];
    int sockfd, userfd;
    int numbytes; 
    socklen_t clilen, addr_size;
    //fd_set master;
   // fd_set read_fds;
    //int fdmax;
    int connection, other_connection;
    bool quit = false;
    int exited = 0;

    int yes = 1;
    //list_init();
    //FD_ZERO(&master);
    //FD_ZERO(&read_fds);
    
    char * port = argv[1]; // get the port 
     
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE; // use my I

    pthread_t thread_listen, thread_command; 
 
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

    //FD_SET(sockfd, &master);
    //fdmax = sockfd;
    printf("one moment please.....\n");
    bool active = true; //the active bool stays true when the userCount > 0
    while(active) {
	struct user *newUser = (struct user *)malloc(sizeof(struct user));
	if ((newUser->sockfd = accept(sockfd, (struct sockaddr *)&client_sock, &addr_size)) == -1) {
	    perror("EROOR: accept");
	    break;
	}
	printf("new connection made on socket %d\n", newUser->sockfd);
	pthread_mutex_lock(&ucnt_lock);
	userCount++;
	pthread_mutex_unlock(&ucnt_lock);
	pthread_create(&(newUser->thread), NULL, message_handler, (void *)newUser);
	if (userCount > 0) {
	    active = true;
	}
	else {
	    active = false;
	}
    }
    printf("closing server\n");
    close(sockfd);
    return 0;
}
        /*memset(buffer, 0, BUF_SIZE);
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1 ) {
            perror("select");
            exit(1);
        }

        for (connection = 0; connection <= fdmax; connection++) {
	        
            //printf("connection: %d\n", connection);
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
		            //printf("message recieved from socket %d\n", connection);
                    if ((numbytes = recv(connection, buffer, BUF_SIZE, 0)) == -1) {
                        perror("ERROR: recv");
                        close(connection);
                        FD_CLR(connection, &master);
                    }
                    if (numbytes == 0) {
                        printf("ignore socket %d\n", connection);
                        close(connection);
                        FD_CLR(connection, &master);
                        quit = true;
                        break;
                    }
                    else {
			printf("going through message_handler: %s\n", buffer);
                        message_handler(connection, buffer, &exited);
			if (exited == 1) {
				close(connection);
				FD_CLR(connection, &master);
				exited = 0;
		    	}
                    }
                }
            }
        }
	if (quit == true) {
	    for (connection = 0; connection <= fdmax; connection++) {
	        if (FD_ISSET(connection, &master)) {
	            printf("ignore socket %d\n", connection);
                    close(connection);
	            FD_CLR(connection, &master);
	        }
            }
            printf("server closed\n");
            return 0;
	}
    }    
}*/
