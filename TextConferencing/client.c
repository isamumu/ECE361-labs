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
#include "packet.h"

#define MAXDATASIZE 4096 //got this number from my ECE344 lab, subject to 
#define MAXBUFLEN 4096
#define BYTE_LIMIT 1000
#define ACK "ACK"
#define NACK "NACK"
#define ATTEMPT_LIMIT 50 //the round trip we have is about 70 usec, want the server to wait for about 3 msec to close, therefore chose 40 resend attempts



void chat(int sockfd) 
{ 
    char buff[MAXBUFLEN]; 
    int n; 
    for (;;) { 
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : "); 
        n = 0; 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
        write(sockfd, buff, sizeof(buff)); 
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
        printf("From Server : %s", buff); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client Exit...\n"); 
            break; 
        } 
    } 
} 

// BIG difference, this lab's about TCP not UDP
int main(int argc, char **argv){
    char *cmd; // will store a line of strings separated by spaces
    char *buff[MAXBUFLEN]; 
    
    // for(;;) is an infinite loop for C like while(1)
    for (;;) { 
        fgets(buf, MAXBUFLEN - 1, stdin); 
        buf[strcspn(buf, "\n")] = 0; // assign the value of the new line to 0
        cmd = buf; 
        
        while(*cmd == ' '){
            cmd++; // if the current input is a space chech the next position
        }

        if(*cmd == 0){ // representation of empty input
            continue; //move on expect the next command string
        }

        cmd = strtok(buf, " "); // break apart command based on spaces
        msgSize = strlen(cmd);

        if (strcmp(cmd, "/login") == 0) {
            // log into the server at a given address and port
            // the IP address is specified in string dot format
			// loginServer();
		} else if (strcmp(cmd, "/logout") == 0) {
            // exit the server
			// logoutServer();
		} else if (strcmp(cmd, "/joinsession") == 0) {
            // join the conference session with the given session id
			// joinsession();
		} else if (strcmp(cmd, "/leavesession") == 0) {
            // leave the currently established session
			// leavesession();
		} else if (strcmp(cmd, "/createsession") == 0) {
            // create a new conference session and join it
			// createsession();
		} else if (strcmp(cmd, "/list") == 0) {
            // get the list of the connected clients and available sessions
			// list();
		} else if (strcmp(cmd, "/quit") == 0) {
            // terminate the program
			// logout();
			break;
		} else{
            // send a message to the current conference session. The message
            // is sent after the new line

            // send();
        }
    } 

    fprintf(stdout, "text conference done.\n");
    return 0;
    
}
