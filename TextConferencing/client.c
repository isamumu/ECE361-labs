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
#include <stdbool.h>
#include "message.h"

#define MAXDATASIZE 4096 //got this number from my ECE344 lab, subject to 
#define MAXBUFLEN 4096
#define BYTE_LIMIT 1000
#define INVALID_SOCKET -1

//bool joined = false;
char buff[MAXBUFLEN];

int serversock = -1;

//This fcn is used in the inet_ntop() for the login fcn as well
void *get_in_addr(struct sockaddr *sock_arr) {
    if (sock_arr->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sock_arr)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sock_arr)->sin6_addr);
}

void *msgRecv(void *arg) {
    printf("new thread created\n");
    int *sockfd = (int *)arg;
    printf("my sockfd: %d\n", *sockfd);
    struct message *recvMsg = (struct message *)malloc(sizeof(struct message));
    int numbytes;
    while(1) {
	if ((numbytes = recv(*sockfd, buff, MAXBUFLEN - 1, 0)) == -1) {
	    perror("ERROR: recv\n");
	    return NULL;
	}
	if (numbytes == 0) {
	    continue;
	}
	recvMsg = formatString(buff);
	if (recvMsg->type == MESSAGE) {
	    printf("%s\n", recvMsg->data);
	}
	else if (recvMsg->type == JN_ACK) {
	    printf("Joined successfully!\n");
	}
	else if (recvMsg->type == NS_ACK) {
	    printf("Added new session and joined successfully!\n");
	}
    }
    return NULL;
} 


int login(char *cmd, int *sockfd, pthread_t *recv_thread){
    char *id, *password, *ip, *port;
    struct addrinfo hints, *servinfo, *p;
    char s[INET6_ADDRSTRLEN];
    int numbytes;
    printf("logging in....\n");
    // extraect the above components from cmd
    cmd = strtok(NULL, " ");
	id = cmd;
	printf("%s\n", id);

	cmd = strtok(NULL, " ");
	password = cmd;
	printf("%s\n", password);

	cmd = strtok(NULL, " ");
	ip = cmd;
	printf("%s\n", ip);

	cmd = strtok(NULL, " \n");
	port = cmd;
	printf("%s\n", port);

    if (id == NULL || password == NULL || ip == NULL || port == NULL) {
		printf("login format: /login <client_id> <password> <server_ip> <server_port>\n");
		return *sockfd;

	}  else if(*sockfd != -1){
        printf("ERROR: attempted multiple logins");
        return *sockfd;

    } else {
        //instantiate TCP protocol
        int dummy;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM; // TCP socket type

	    hints.ai_protocol = IPPROTO_TCP;
    	hints.ai_flags = AI_PASSIVE; // use my I

        // find the IP at the specified port (FIND THE SERVER)
        if ((dummy = getaddrinfo(ip, port, &hints, &servinfo)) != 0) {
            perror("bad retrieval");
            return -1;
        }
        
        // referenced from Beej's code pg.35
        // get socket from server port
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((*sockfd = socket(p->ai_family, p->ai_socktype,
                servinfo->ai_protocol)) == -1) {
                perror("client: socket\n");
                continue;
            }

            if (connect(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(*sockfd);
                perror("client: connect\n");
                continue;
            }
            
            break;
        }

        if (p == NULL) {
            printf("client: failed to connect\n");
            close(*sockfd);
            *sockfd = -1; // make the socket variable reusable for next connection 
            return *sockfd;
        }

        // at this point we have the socket, so let's connect!
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
        printf("client: connecting to %s\n", s);
        freeaddrinfo(servinfo); // all done with this structure

        int numbytes;
        struct message *msg = (struct message *)malloc(sizeof(struct message));

        msg->type = LOGIN;
        strncpy(msg->source, id, MAX_NAME);
        strncpy(msg->data, password, MAX_DATA);
        msg->size = strlen(msg->data);
        
	    memset(buff, 0, BUF_SIZE);
        formatMessage(msg, buff);
        printf("login message formed: %s\n", buff);
        //serversock = *sockfd;
        //print_message(msg);
        if((numbytes = send(*sockfd, buff, MAXBUFLEN - 1, 0)) == -1){
            fprintf(stderr, "login error\n");
			close(*sockfd);
            *sockfd = -1;
			return -1;
        }
        
        // MESSAGE TYPES: 
        // --> LO_ACK - login successful
        // --> LO_NAK - login unsuccessful
        
        if ((numbytes = recv(*sockfd, buff, MAXBUFLEN - 1, 0)) == -1) {
            fprintf(stderr, "ERROR: nothing received\n");
            return -1;
        }
        
	
        buff[numbytes] = 0;
        msg = formatString(buff);

        if(msg->type == LO_ACK){
            fprintf(stdout, "login success!\n");
	    pthread_create(recv_thread, NULL, msgRecv, sockfd);
	    return *sockfd;

        } else if (msg->type == LO_NACK) {
            fprintf(stdout, "login failure b/c %s\n", msg->data);
            close(*sockfd);
            *sockfd = -1;

            return *sockfd;
        } else {
            fprintf(stdout, "INVALID INPUT: type %d, data %s\n", msg->type, msg->data);
            //close(socketfd_p);
            //*socketfd_p = INVALID_SOCKET;
            close(*sockfd);
            *sockfd = -1;
            return *sockfd;
        } 
    }
}

void logout(int sockfd){
    if(sockfd == -1) {
        fprintf(stdout, "Currently no logins found.\n");
        return;
    }

    int numbytes;
    struct message *msg;
    msg->type = EXIT;
    msg->size = 0;

    formatMessage(msg, buff);

    if((numbytes = send(sockfd, buff, MAXBUFLEN - 1, 0)) == -1){
        fprintf(stderr, "logout error\n");
        return;
    }

    //joined = false;
    close(sockfd);
    sockfd = -1;
    printf("logged out\n");
    return;

}

void joinsession(char *session, int sockfd) {
    if (session == NULL) {
        fprintf(stdout, "invalid session id, input format: /joinsession <session ID>\n");
        return;
    } else if (sockfd == -1){
        fprintf(stdout, "Please login to a server before trying to join a session\n");
        return;

    } else {
        struct message *newMessage = (struct message *)malloc(sizeof(struct message));
        newMessage->type = JOIN;
        newMessage->size = strlen(session);
        strncpy(newMessage->data, session, MAX_DATA);
	
        int bytes;

        formatMessage(newMessage, buff);
        if ((bytes = send(sockfd, buff, MAXBUFLEN, 0)) == -1) {
            fprintf(stdout, "ERROR: send() failed\n");
            return;
        }

        /*if ((bytes = recv(sockfd, buff, MAXBUFLEN - 1, 0)) == -1) {
			fprintf(stderr, "ERROR: nothing received\n");
			return;
		}

        buff[bytes] = 0; // mark end of the string
        newMessage = formatString(buff);

        if (newMessage->type == JN_ACK) {
            fprintf(stdout, "Join Successful!\n");
            //joined = true;
        } else if (newMessage->type == JN_NACK) {
            fprintf(stdout, "Join Failed... Data: %s\n", newMessage->source);
            //joined = false;
	}*/
        return;
    }
}

void leavesession(char *session, int sockfd) {
    if (sockfd == -1) {
        fprintf(stdout, "Please login to a server before trying to join a session\n");
        return;
    } else {
	printf("leaving sessions\n");
        struct message *newMessage = (struct message *)malloc(sizeof(struct message));
        newMessage->type = LEAVE_SESS;
        newMessage->size = 0;
        strncpy(newMessage->data, session, MAX_DATA); // tell the server which server to leave

        int bytes;

        formatMessage(newMessage, buff);
	printf("message sent: %s\n", buff);

        if ((bytes = send(sockfd, buff, MAXBUFLEN, 0)) == -1) {
            fprintf(stdout, "ERROR: send() failed\n");
            return;
        }
	//joined = false;
	printf("leave session success\n");
        return;
    }
}

void createsession(char *session, int sockfd) {
    printf("creating new session\n");
    if (session == NULL) {
        printf("invalid session id, input format: /createsession <session ID>\n");
        return;
    } else if (sockfd == INVALID_SOCKET) {
        printf("Please login to a server before trying to join a session\n");
        return;
    } else {
        struct message *newMessage = (struct message *)malloc(sizeof(struct message));
        newMessage->type = NEW_SESS;
        newMessage->size = strlen(session);
        
        strncpy(newMessage->data, session, MAX_DATA);
        int bytes;

        formatMessage(newMessage, buff);
        if ((bytes = send(sockfd, buff, MAXBUFLEN, 0)) == -1) {
            printf("ERROR: send() failed\n");
            return;
        }

        /*if ((bytes = recv(sockfd, buff, MAXBUFLEN - 1, 0)) == -1) {
			printf("ERROR: nothing received\n");
			return;
		}

        buff[bytes] = 0; // mark end of the string
        newMessage = formatString(buff);
        
        if (newMessage->type == NS_ACK) {
            printf("Successfully created and joined session %s.\n", newMessage->data);
            //joined = true;
        }*/
        return;
    }
}

void list(char* session, int sockfd) {
    if (sockfd == INVALID_SOCKET) {
        fprintf(stdout, "Please login to a server before trying to join a session\n");
        return;
    } else {
        struct message *newMessage;
        newMessage->type = QUERY;
        newMessage->size = 0;
        strncpy(newMessage->data, session, MAX_DATA); // tell the server which server to list

        int bytes;
        formatMessage(newMessage, buff);

        if ((bytes = send(sockfd, buff, MAXBUFLEN, 0)) == -1) {
            fprintf(stdout, "ERROR: send() failed\n");
            return;
        }

        if ((bytes = recv(sockfd, buff, MAXBUFLEN - 1, 0)) == -1) {
			fprintf(stderr, "ERROR: nothing received\n");
			return;
		}

        buff[bytes] = 0; // mark end of the string
        //printf("the buffer %s: \n", buff);
        newMessage = formatString(buff);

        if (newMessage->type == QU_ACK) {
            //printf(newMessage->data);
            fprintf(stdout, "User id & Session ids\n%s", newMessage->source);
        }

        return;
    }
}

void sendMsg(int sockfd){
    if(sockfd == INVALID_SOCKET){
        fprintf(stdout, "Please login to a server before trying to join a session\n");
        return;
    } 

    // stage 2. send message in that session
    printf("sending message: %s\n", buff);
    int numbytes;
    struct message *msg = (struct message *)malloc(sizeof(struct message));
    msg->type = MESSAGE;
    // the receiver should based on this target session locate the right socket to send to

    strncpy(msg->data, buff, MAX_DATA);
    msg->size = strlen(msg->data);
    memset(buff, 0, MAXBUFLEN);
    formatMessage(msg, buff);
    printf("message sent: %s\n", buff);

    if((numbytes = send(sockfd, buff, MAXBUFLEN - 1, 0)) == -1){
        fprintf(stderr, "send error\n");
        return;
    }
}

void quit(int sockfd) {
    if (sockfd == INVALID_SOCKET) {
	printf("Please login to a server before trying to join a session\n");
	return;
    }
    memset(buff, 0, BUF_SIZE);
    int numbytes;
    if((numbytes = send(sockfd, buff, MAXBUFLEN - 1, 0)) == -1){
        fprintf(stderr, "send error\n");
        return;
    }
    printf("program quitted\n");
}

void invite(char *cmd, int sockfd) {
    if (sockfd == INVALID_SOCKET) {
        printf("Please login to a server before trying to join a session\n");
        return;
    }

    int numbytes;
    struct message *msg = (struct message *)malloc(sizeof(struct message));
    char *src = strtok(NULL, " "); //cmd should contain the session id
    char *invitee = strtok(NULL, " "); //cmd should contain the session id

    msg->type = INVITE;
    char *invitation = strcat(invitee, " has been invited to ");
    invitation = strcat(invitation, "join src (Y/N)");

    printf("invitation: %s", invitation);

    strncpy(msg->data, invitation, MAX_DATA);
    msg->size = strlen(msg->data);
    memset(buff, 0, MAXBUFLEN);
    formatMessage(msg, buff);

    if((numbytes = send(sockfd, buff, MAXBUFLEN - 1, 0)) == -1){
        fprintf(stderr, "send error\n");
        return;
    }
    
}

void acceptReq(char* response, char* session, char* user, int sockfd) {
    if (sockfd == INVALID_SOCKET) {
        printf("Please login to a server before trying to join a session\n");
        return;
    }

    int numbytes;
    struct message *msg = (struct message *)malloc(sizeof(struct message));
    msg->type = ACCEPT;

    // the receiver should based on this target session locate the right socket to send to
    formatMessage(msg, buff);

    if((numbytes = send(sockfd, buff, MAXBUFLEN - 1, 0)) == -1){
        fprintf(stderr, "send error\n");
        return;
    }
    
}

/*
PART 2 Objectives (Client):
--> Allow  a  client  to join  multiple  sessions.  If  so,  you  should  clearly  indicate  on  the client’s terminal the session identification of every message. 
------> check joinSession and allow for multiple sessions.
------> do i need multithreading? I don't think so...
------> create a new field in user to document which sessions they have joined. ie each user should have a linked list for joined sessions
--> Implement a procedure for a client to invite other clients into a session. If so, you must provide a protocol for a client to either accept or refuse an invitation
------> create a protocol similar to lab 1. client1 (join invite) -> server (forward) -> client2 (resp) -> server (forward) -> client1 (Y: joinsession for client2 sock OR N: do nothing)

NOTE: now the client takes in a session field which indicates which session the user wants to leave, list, or send messages at

PART 2 Objectives (Server):
--> Keep all sessions that theclient joined. If one client could join multiple sessions, you should carefully design the up-to-date list.
--> If one client could invite other clients into a session, the server should be able to forward the invitation and corresponding messages to the specific clients. 
--> You may wish to use a timerwith each client, to disconnect clients that have been inactive for a long time.
*/

// BIG difference, this lab's about TCP not UDP
int main(int argc, char **argv){

    char *cmd; // will store a line of strings separated by spaces   
    int sockfd = INVALID_SOCKET; // init socket value
    int len;
    int bytes;
    struct message *msg = (struct message *)malloc(sizeof(struct message));
    pthread_t thread;
    

    // for(;;) is an infinite loop for C like while(1)
    for (;;) { 

        fgets(buff, MAXBUFLEN - 1, stdin); 
        // TODO: CHECK buff reset
        buff[strcspn(buff, "\n")] = 0; // assign the value of the new line to 0
        cmd = buff; 
        
        while(*cmd == ' '){
            cmd++; // if the current input is a space chech the next position
        }

        if(*cmd == 0){ // representation of empty input
            continue; //move on expect the next command string
        }

        cmd = strtok(buff, " "); // break apart command based on spaces
        len = strlen(cmd);

        if (strcmp(cmd, "/login") == 0) {
            // log into the server at a given address and port
            // the IP address is specified in string dot format
			sockfd = login(cmd, &sockfd, &thread);

		} else if (strcmp(cmd, "/logout") == 0) {
            // exit the server
			logout(sockfd);
			break;

		} else if (strcmp(cmd, "/joinsession") == 0) {
            // join the conference session with the given session id
            cmd = strtok(NULL, " "); //cmd should contain the session id
			printf("session name: %s\n", cmd);
			joinsession(cmd, sockfd);

		} else if (strcmp(cmd, "/leavesession") == 0) {
            // leave the currently established session
            leavesession("all", sockfd);
            
		} else if (strcmp(cmd, "/createsession") == 0) {
            // create a new conference session and join it
			cmd = strtok(NULL, " "); //cmd should contain the session id
			createsession(cmd, sockfd);

		} else if (strcmp(cmd, "/list") == 0) {
            // get the list of the connected clients and available sessions
			//list(sockfd);

		} else if (strcmp(cmd, "/quit") == 0) {
            // terminate the program
			quit(sockfd);
			break;
        
        } else if (strcmp(cmd, "/invite") == 0) {
            // terminate the program
			
			printf("session name: %s\n", cmd);
			invite(cmd, sockfd);

		} else {
            // send a message to the current conference session. The message
            // is sent after the new line
            buff[len] = ' ';
            sendMsg(sockfd);
        }
	/*
	if(serversock != -1){
	    struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 999999;
            // sockopt reconfigures the socket, must be done in order to measure the timeout
            if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&timeout, sizeof(timeout)) == -1) {
                printf("failed to configure socket\n");
	        exit(1);
            }
            if ((bytes = recv(serversock, buff, MAXBUFLEN - 1, 0)) == -1) {
                continue;
            }
            //msg = formatString(buff);
	    printf("data: %s\n", buff);
            printf("message recieved: %s\n", buff);
        }*/
    }

    fprintf(stdout, "text conference done.\n");
    return 0;
}
