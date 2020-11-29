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
#define MAXBUFLEN 4096

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
    bool istimeout = false;


    // TODO after creating linked list functions, implement 
    // Hannah: session functions (create join and leave)
    // Isamu: rest

    while (1) {
	memset(buff, 0, BUF_SIZE);
	//timer
        struct timeval timeout;
        timeout.tv_sec = 600; //10mins
        timeout.tv_usec = 0;
        setsockopt(newUser->sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout));
	if ((numbytes = recv(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
	    istimeout = true;
	    perror("ERROR: recv");
	    break;
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
                    addUser(&user_list, myuser);
	            pthread_mutex_unlock(&users_lock);
		    isLogin = true;
	            printf("user %s logged in\n", newUser->name);
		    //printUsers(user_list);
                } 
	        else {
                    respMsg->type = LO_NACK;
		    memset(respMsg->data, 0, MAX_DATA);
                    char tmp[100] = "Wrong username or password";
                    strcpy(respMsg->data, tmp);
                    respMsg->size = strlen(respMsg->data);
                    printf("message sent: %s\n", respMsg->data);
		    exited = true;
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
		memset(respMsg->data, 0, MAX_DATA);
                char tmp[100] = "Please login first";
                strcpy(respMsg->data, tmp);
                respMsg->size = strlen(respMsg->data);
                printf("message sent: %s\n", respMsg->data);
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
                memset(respMsg->data, 0, MAX_DATA);
                char tmp[100] = "Session not found";
                strcpy(respMsg->data, tmp);
                respMsg->size = strlen(respMsg->data);
                printf("message sent: %s\n", respMsg->data);
            }
	        else {

                newUser->sessionID = newMsg->data;
                respMsg->type = JN_ACK;
                strncpy(respMsg->data, newMsg->data, MAX_DATA);
                struct user *myuser = (struct user *)malloc(sizeof(struct user));

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
	    if (strcmp(newMsg->data, "all") == 0) {
		struct session *session_found = findSessUser(session_list, newUser);
		respMsg->type = LS_ACK;
	        while (session_found != NULL) {
		    pthread_mutex_lock(&sessions_lock);
	            removeSessUser(session_found, newUser, &session_list);
		    pthread_mutex_unlock(&sessions_lock);
		    session_found = findSessUser(session_list, newUser);
	        }
	    }
	    else {
	        struct session *session_found = findSession(session_list, newMsg->data);
	        if (session_found != NULL) {
		    struct user *user_found = findUser(session_found->users, newUser->sockfd);
		    if (user_found == NULL) {
			respMsg->type = LS_NACK;
		        memset(respMsg->data, 0, MAX_DATA);
                        char tmp[100] = "Session not joined";
                        strcpy(respMsg->data, tmp);
                        respMsg->size = strlen(respMsg->data);
                        printf("message sent: %s\n", respMsg->data);
		    }
		    else {
		        respMsg->type = LS_ACK;
		        pthread_mutex_lock(&sessions_lock);
	                removeSessUser(session_found, newUser, &session_list);
		        pthread_mutex_unlock(&sessions_lock);
		    }
	        }
	        else {
		    respMsg->type = LS_NACK;
		    memset(respMsg->data, 0, MAX_DATA);
                    char tmp[100] = "Session does not exist";
                    strcpy(respMsg->data, tmp);
                    respMsg->size = strlen(respMsg->data);
                    printf("message sent: %s\n", respMsg->data);
	        }
	    }
	    formatMessage(respMsg, buff);
	    if ((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
                fprintf(stderr, "ACK error\n");
            }
	    printSessions(session_list);
	    printUsers(user_list);
        } 
        else if (newMsg->type == NEW_SESS) {
            printf("NEW_SESS recieved for user: %s\n", newUser->name);
            struct session *session_found = findSession(session_list, newMsg->data);
	    if (session_found == NULL) {
                if (newUser->sessionID == NULL) {
		    print_message(newMsg);
		    newUser->sessionID = (char *)malloc(strlen(newMsg->data) + 1);
		    strcpy(newUser->sessionID, newMsg->data);
	        }
	        struct session *newSession = (struct session *)malloc(sizeof(struct session));
                print_message(newMsg);
                //newUser->sessionID = (char *)malloc(strlen(newMsg->data) + 1);
	        //strcpy(newUser->sessionID, newMsg->data);
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
	    else {
		respMsg->type = NS_NACK;
		memset(respMsg->data, 0, MAX_DATA);
                char tmp[100] = "Session already exist";
                strcpy(respMsg->data, tmp);
                respMsg->size = strlen(respMsg->data);
                printf("message sent: %s\n", respMsg->data);
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
            //printf("session: %s\n", targetSession);
            char *text = strtok(NULL, ",");
	    respMsg->type = MESSAGE;
            //printf("text: %s\n", text);
	    if (text == NULL) {
		memset(respMsg->data, 0, MAX_DATA);
                char tmp[100] = "Wrong format <session>,<message>";
                strcpy(respMsg->data, tmp);
                respMsg->size = strlen(respMsg->data);
                formatMessage(respMsg, buff);
                printf("message sent: %s\n", respMsg->data);
                printf("wrong format\n");

                if ((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
                    perror("ERROR: send\n");
                    exit(1);
                }
	    }
	    else {
                struct user *myUser;
                strcpy(respMsg->data, text);
                respMsg->size = strlen(respMsg->data);
                //formatMessage(respMsg, buff);
                struct session *session_found = findSession(session_list, targetSession);
                if (session_found == NULL) {
                    memset(respMsg->data, 0, MAX_DATA);
                    char tmp[100] = "Wrong Session name";
                    strcpy(respMsg->data, tmp);
                    respMsg->size = strlen(respMsg->data);
                    formatMessage(respMsg, buff);
                    printf("message sent: %s\n", respMsg->data);
                    printf("wrong session name\n");

                    if ((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
                        perror("ERROR: send\n");
                        exit(1);
                    }
                }
                else {
                    struct user *isRegister = findUser(session_found->users, newUser->sockfd);
		    if (isRegister == NULL) {
		        memset(respMsg->data, 0, MAX_DATA);
                        char tmp[100] = "Not joined in the Session, please join first";
                        strcpy(respMsg->data, tmp);
                        respMsg->size = strlen(respMsg->data);
                        formatMessage(respMsg, buff);
                        printf("message sent: %s\n", respMsg->data);
                        printf("not joined\n");

                        if ((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
                            perror("ERROR: send\n");
                            exit(1);
                        }
		    }
		    else {
                        // locate the session and its users
                        formatMessage(respMsg, buff);
                        struct user *ptr = session_found->users;
                        printf("message: %s\n", respMsg->data);

                        // send to every user in the session
                        while(ptr != NULL) {
		            if (ptr->sockfd != newUser->sockfd) {
                                printf("sending message to user %s\n", ptr->name);
                                if ((numbytes = send(ptr->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
                                    perror("ERROR: send\n");
                                    exit(1);
                                }
		            }
                            ptr = ptr->next;
                        }
		    }
                }
	    }
        }

        else if (newMsg->type == INVITE) {
            // obtain the invited session and the user to invite
            printf("incoming invitation from %s\n", newUser->name);
            //char *invitee= strtok(newMsg->data, ",");
            printf("invited: %s\n", newMsg->source);
            //char *src = strtok(NULL, ",");
            printf("session: %s\n", newMsg->data);
	    bool found = false;

            // find the socket of the user
            struct user *nominee = findUserName(user_list, newMsg->source);
	    struct session *session_found = findSession(session_list, newMsg->data);
	    if (nominee == NULL) {
		respMsg->type = INV_NACK_U;
		strcpy(respMsg->data, newMsg->source);
		respMsg->size = strlen(respMsg->data);
		printf("nominee: %s not found\n", newMsg->source);
	    }
	    else if (session_found == NULL) {
		respMsg->type = INV_NACK_S;
		strcpy(respMsg->data, newMsg->data);
		respMsg->size = strlen(respMsg->data);
		printf("session: %s not found\n", newMsg->data);
	    }
	    else {
                printf("nominee is found: %s\n", nominee->name);
		printf("session is found: %s\n", session_found->sessionName);
		found = true;
                respMsg->type = INVITE;
                strcpy(respMsg->data, newMsg->data);
                respMsg->size = strlen(respMsg->data);
	    }
            formatMessage(respMsg, buff);
	    int numbytes;
            //printf("yoohoo\n");
	    if (found) {
                if((numbytes = send(nominee->sockfd, buff, MAXBUFLEN - 1, 0)) == -1){
                    fprintf(stderr, "send error\n");
                    return 0;
                }
	    }
	    else {
                if((numbytes = send(newUser->sockfd, buff, MAXBUFLEN - 1, 0)) == -1){
                    fprintf(stderr, "send error\n");
                    return 0;
                }
	    }

        }
        else if (newMsg->type == QUERY) {
            //print out the list of user and avaliable sessions
            //send back ACK
	    printf("QUERY recieved for user: %s\n", newUser->name);
	    if (strcmp(newMsg->data, "all") == 0) {
		respMsg->type = QU_ACK;
                struct session *ptr = session_list;
            
                char data[MAX_DATA] = "";
                while (ptr != NULL) {
                    if (ptr->sessionName == NULL) {
                        ptr = ptr->next;
                        continue;
                   }
                    strcat(data, ptr->sessionName);
                    strcat(data, "->");
                    struct user *people = ptr->users;
                    while(people != NULL){
                        if(people->name != NULL){
                            strcat(data, people->name);
                            if(people->next != NULL){
                                strcat(data, ", ");
                            }
                        }
                        people = people->next;
                    }
                    strcat(data, "\n");
                    ptr = ptr->next;
               }
		strncpy(respMsg->source, data, MAX_DATA);
	    }
	    else {
	        struct session *session_found = findSession(session_list, newMsg->data);
	        if (session_found == NULL) {
		    respMsg->type = QU_NACK;
		    memset(respMsg->data, 0, MAX_DATA);
                    char tmp[100] = "Session does not exist";
                    strcpy(respMsg->data, tmp);
                    respMsg->size = strlen(respMsg->data);
                    printf("message sent: %s\n", respMsg->data);
	        }
		else {
		    struct user *user_found = findUser(session_found->users, newUser->sockfd);
		    if (user_found == NULL) {
			respMsg->type = QU_NACK;
		        memset(respMsg->data, 0, MAX_DATA);
                        char tmp[100] = "Session not joined";
                        strcpy(respMsg->data, tmp);
                        respMsg->size = strlen(respMsg->data);
                        printf("message sent: %s\n", respMsg->data);
		    }
		    else {
            	        respMsg->type = QU_ACK;
            
                        char data[MAX_DATA] = "";
                        strcat(data, session_found->sessionName);
                        strcat(data, "->");
                        struct user *people = session_found->users;
                        while(people != NULL){
                            if(people->name != NULL){
                                strcat(data, people->name);
                                if(people->next != NULL){
                                    strcat(data, ", ");
                                }
                            }
                            people = people->next;
                        }
                        strcat(data, "\n");
		        strncpy(respMsg->source, data, MAX_DATA);
		    }
		}
	    }
        
            //printf("data: %s", data);
            //strncpy(respMsg->data, data, MAX_DATA);
            //printf("source: %s\n", respMsg->data);
            memset(buff, 0, BUF_SIZE);
            formatMessage(respMsg, buff);

            if((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1){
                fprintf(stderr, "ACK error\n");
		return 0;
            }
        }
	if (exited) {
            break;
	}
    }
    printf("user: %s ", newUser->name);
    if (istimeout) {
	respMsg->type = TIMEOUT;
	respMsg->size = 0;
	formatMessage(respMsg, buff);
	if ((numbytes = send(newUser->sockfd, buff, BUF_SIZE - 1, 0)) == -1) {
	    perror("ERROR: send ");
	}
    }

    close(newUser->sockfd);
    if (isLogin) {
	pthread_mutex_lock(&users_lock);
	removeUser(&user_list, newUser);
	pthread_mutex_unlock(&users_lock);
	struct session *session_found = findSessUser(session_list, newUser);
	while (session_found != NULL) {
            pthread_mutex_lock(&sessions_lock);
	    removeSessUser(session_found, newUser, &session_list);
	    pthread_mutex_unlock(&sessions_lock);
	    session_found = findSessUser(session_list, newUser);
	}

	pthread_mutex_lock(&ucnt_lock);
	userCount--;
	pthread_mutex_unlock(&ucnt_lock);
	printf("successfully quitted the program\n");
    }
    else {
        printf("Please login\n");
    }
    isLogin = false;
    pthread_cancel(newUser->thread);
    free(newUser);
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
    int connection, other_connection;
    bool quit = false;
    int exited = 0;

    int yes = 1;
    
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

    //timer
    struct timeval timeout;
    timeout.tv_sec = 600; //10mins
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout));

    if ((numbytes = listen(sockfd, BACKLOG)) == -1) {
        perror("server: listen");
        exit(1);
    }
    bool active = true;
    int sockCheck = -1;

    printf("one moment please.....\n");
    while(1) {
	struct user *newUser = (struct user *)malloc(sizeof(struct user));
	if ((sockCheck = accept(sockfd, (struct sockaddr *)&client_sock, &addr_size)) == -1) {
	    //perror("EROOR: accept");
	    if (userCount == 0) {
		active = false;
	    }
	    //break;
	}
	if (sockCheck != -1) {
	newUser->sockfd = sockCheck;
	printf("new connection made on socket %d\n", newUser->sockfd);
	pthread_mutex_lock(&ucnt_lock);
	userCount++;
	pthread_mutex_unlock(&ucnt_lock);
	pthread_create(&(newUser->thread), NULL, message_handler, (void *)newUser);
	}
	if (!active) {
	    break;
	}
    }
    printf("closing server\n");
    close(sockfd);
    return 0;
}
