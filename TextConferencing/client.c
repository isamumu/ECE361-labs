#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <message.h>
#include <time.h>

#include "message.h"

#define MAXDATASIZE 4096 //got this number from my ECE344 lab, subject to 
#define MAXBUFLEN 4096
#define BYTE_LIMIT 1000

bool joined = false;
char buff[MAXBUFLEN];


void login(char *cmd, int *sockfd){
    char *id, *password, *ip, *port;
    struct addrinfo hints, *servinfo, *p;
    char s[INET6_ADDRSTRLEN];
    int numbytes;
    // extraect the above components from cmd

    cmd = strtok(NULL, " ");
	id = cmd;

	cmd = strtok(NULL, " ");
	password = cmd;

	cmd = strtok(NULL, " ");
	ip = cmd;

	cmd = strtok(NULL, " \n");
	port = cmd;

    if (id == NULL || password == NULL || ip == NULL || port == NULL) {
		fprintf(stdout, "login format: /login <client_id> <password> <server_ip> <server_port>\n");
		return 1;

	}  else if(*socket != INVALID_SOCKET){
        fprintf("ERROR: attempted multiple logins")
        return 1;

    } else {
        //instantiate TCP protocol
        int dummy;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM; // TCP socket type

        // find the IP at the specified port (FIND THE SERVER)
        if ((dummy = getaddrinfo(argv[1], port, &hints, &servinfo)) != 0) {
            perror("bad retrieval");
            return 1;
        }
        
        // referenced from Beej's code pg.35
        // get socket from server port
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
                perror("client: socket\n");
                continue;
            }

            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                perror("client: connect\n");
                continue;
            }
            
            break;
        }

        if (p == NULL) {
            fprintf(stderr, "client: failed to connect\n");
            close(sockfd);
            *sockfd = INVALID_SOCKET; // make the socket variable reusable for next connection 
            return 1;
        }

        // at this point we have the socket, so let's connect!
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
        printf("client: connecting to %s\n", s);
        freeaddrinfo(servinfo); // all done with this structure

        int numbytes;
        struct message msg;

        msg.type = LOGIN;
        strncpy(msg.source, id, MAX_NAME);
        strncpy(msg.data, password, MAX_DATA);
        msg.size = strlen(msg.data);
        
        formatMessage(msg, buff);

        if((numbytes = send(*sockfd, buff, MAXBUFLEN - 1, 0)) == -1){
            fprintf(stderr, "login error\n");
			close(sockfd);
            *sockfd = INVALID_SOCKET;
			return;
        }
        
        // MESSAGE TYPES: 
        // --> LO_ACK - login successful
        // --> LO_NAK - login unsuccessful
        buff[numbytes] = 0;
        if(msg.type = LO_ACK && pthread_create(receive_thread_p, NULL, receive, socketfd_p) == 0)){
            fprintf(stdout, "login success!\n");

        } else if (msg.type == LO_NAK) {
            fprintf(stdout, "login failure b/c %s\n", msg.data);
            close(sockfd);
            *sockfd = INVALID_SOCKET;

			return;
        } else {
            fprintf(stdout, "INVALID INPUT: type %d, data %s\n", msg.type, msg.data);
            close(sockfd_p);
            *sockfd_p = INVALID_SOCKET;
            return;
        } 
    }
}

void logout(int *sockfd, pthread_t *rcv_thread){
    if(*sockfd == INVALID_SOCKET) {
        fprintf(stdout, "Currently no logins found.\n");
        return;
    }

    int numbytes;
    struct message msg;
    msg.type = EXIT;
    msg.size = 0;

    formatMessage(msg, buff);

    if((numbytes = send(*sockfd, buff, MAXBUFLEN - 1, 0)) == -1){
        fprintf(stderr, "logout error\n");
        return;
    }

    if (pthread_cancel(*rcv_thread)) {
        fprintf(stderr, "logout failed\n");
	} else {
        fprintf(stdout, "logout success!\n");	
	} 

    joined = false;
    close(sockfd);
    *sockfd = INVALID_SOCKET;

}

void joinsession(char *session, int sockfd) {
    if (session == NULL) {
        fprintf(stdout, "invalid session id, input format: /joinsession <session ID>\n");
        return;
    } else if (sockfd == -1){
        fprintf(stdout, "Please login to a server before trying to join a session\n");
        return;

    } else if(joined){
        fprintf(stdout, "can't join multiple sessions")
        return;

    } else {
        struct message newMessage;
        newMessage.type = JOIN;
        newMessage.size = strlen(session);
        strcpy(newMessage.data, session, MAX_DATA);
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
        newMessage = formatString(buff);

        if (newMessage.type == JN_ACK) {
            fprintf(stdout, "Join Success... session ID: %s.\n", newMessage.data);
            joined = true;
        } else if (packet.type == JN_NAK) {
            fprintf(stdout, "Join Failed... Data: %s\n", newMessage.data);
            joined = false;

        return;
    }
}

void leavesession(int sockfd) {
    if (sockfd == -1) {
        fprintf(stdout, "Please login to a server before trying to join a session\n");
        return;
    } else {
        struct message newMessage;
        newMessage.type = LEAVE_SESS;
        newMessage.size = 0;
        //strcpy(newMessage.data, session, MAX_DATA);
        int bytes;

        formatMessage(newMessage, buff);

        if ((bytes = send(sockfd, buff, MAXBUFLEN, 0)) == -1) {
            fprintf(stdout, "ERROR: send() failed\n");
            return;
        }
        return;
    }
}

void createsession(char *session, int sockfd) {
    if (session == NULL) {
        fprintf(stdout, "invalid session id, input format: /createsession <session ID>\n");
        return;
    } else if (sockfd == INVALID_SOCKET) {
        fprintf(stdout, "Please login to a server before trying to join a session\n");
        return;
    } else {
        struct message newMessage;
        newMessage.type = NEW_SESS;
        newMessage.size = 0;
        //strcpy(newMessage.data, session, MAX_DATA);
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
        newMessage = formatString(buff);

        if (newMessage.type == NS_ACK) {
            fprintf(stdout, "Successfully created and joined session %s.\n", newMessage.data);
            joined = true;
        }

        return;
    }
}

void list(int sockfd) {
    if (sockfd == INVALID_SOCKET) {
        fprintf(stdout, "Please login to a server before trying to join a session\n");
        return;
    } else {
        struct message newMessage;
        newMessage.type = QUERY;
        newMessage.size = 0;
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
        newMessage = formatString(buff);

        if (newMessage.type == QU_ACK) {
            fprintf(stdout, "User id\t\tSession ids\n%s", newMessage.data);
        }

        return;
    }
}

void send(int sockfd){
    if(sockfd == INVALID_SOCKET){
        fprintf(stdout, "Please login to a server before trying to join a session\n");
        return;
    } else if(!joined){
        fprintf(stdout, "Please login to a session before trying to join a session\n");
        return;
    }

    int numbytes;
    struct message msg;
    msg.type = MESSAGE;

    strncpy(msg.data, buff, MAX_DATA);
    msg.size = strlen(msg.data);
    formatMessage(msg, buff);

    if((numbytes = send(sockfd, buff, MAXBUFLEN - 1, 0)) == -1){
        fprintf(stderr, "send error\n");
        return
    }

    
}


// BIG difference, this lab's about TCP not UDP
int main(int argc, char **argv){
    char *cmd; // will store a line of strings separated by spaces   
    int sockfd = INVALID_SOCKET; // init socket value
    pthread_t rcv_msg;
    int len;
    
    // for(;;) is an infinite loop for C like while(1)
    for (;;) { 
        fgets(buff, MAXBUFLEN - 1, stdin); 
        // TODO: CHECK buff reset
        buff[strcspn(buf, "\n")] = 0; // assign the value of the new line to 0
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
			login(cmd, &sockfd);

		} else if (strcmp(cmd, "/logout") == 0) {
            // exit the server
			logout(&sockfd, rcv_msg);

		} else if (strcmp(cmd, "/joinsession") == 0) {
            // join the conference session with the given session id
            cmd = strtok(NULL, " "); //cmd should contain the session id
			joinsession(cmd, &sockfd);

		} else if (strcmp(cmd, "/leavesession") == 0) {
            // leave the currently established session
            leavesession(sockfd);

		} else if (strcmp(cmd, "/createsession") == 0) {
            // create a new conference session and join it
			cmd = strtok(NULL, " "); //cmd should contain the session id
            createsession(sockfd);

		} else if (strcmp(cmd, "/list") == 0) {
            // get the list of the connected clients and available sessions
			list(sockfd);

		} else if (strcmp(cmd, "/quit") == 0) {
            // terminate the program
			logout(&sockfd, rcv_msg);
			break;

		} else{
            // send a message to the current conference session. The message
            // is sent after the new line
            buff[len] = ' ';
            send(sockfd);
        }
    } 

    fprintf(stdout, "text conference done.\n");
    return 0;
    
}
