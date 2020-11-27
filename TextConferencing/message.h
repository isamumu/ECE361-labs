#include <pthread.h>

#define MAX_NAME 1100
#define MAX_DATA 4096
#define BUF_SIZE 1100
#define SIZE 20

enum messageType {
	LOGIN,
	LO_ACK,
	LO_NACK,
	EXIT,
	JOIN,
	JN_ACK,
	JN_NACK,
	LEAVE_SESS,
	NEW_SESS,
	NS_ACK,
	MESSAGE,
	QUERY,
	QU_ACK,
};

struct message {
    unsigned int type;
    unsigned int size;
    unsigned char source[MAX_NAME];
    unsigned char data[MAX_DATA];
};

// for each joined user
struct user {
    pthread_t thread;
    char name[MAX_NAME];
    char password[MAX_NAME];
    char *sessionID;
    int sockfd; // used as ref to send to every user in the linked list
    struct user *next;// = NULL; // NOTE: init to null here
    int user_cnt;// = -1; //change on the dummy to 0
};

struct account {
	char id[MAX_NAME];
	char password[MAX_NAME];
	struct account *next;

};

// for each session created
struct session {
	char *sessionName;
	//int session_number;
	//int socketfds[10];
	//int user_cnt = 0; //change on the dummy
	int session_cnt;// = -1; //change on first node
	struct user *users;
	struct session *next;// = NULL; // i think this is good practice?
};

bool findAcct(struct account *head, char *username, char *password){
	struct account *ptr;
	ptr = head;

	if(ptr == NULL){
		return false;
	} else{
		while(ptr != NULL){
			if(strcmp(ptr->password, password) == 0 && strcmp(ptr->id, username) == 0){
				return true;
			} else{
				//struct account *temp = ptr->next;
				ptr = ptr->next;
			}
		}
	}
	
	return false;
}

// TODO Hannah
void addSession(struct session *head, struct session *mySession) {
	if (head == NULL) {
		head = mySession;
		return;
	}
	struct session *ptr = head;
	while (ptr->next != NULL) {
		ptr = ptr->next;
	}
	//mySession->users->user_cnt = 0;

	ptr->next = mySession;
	mySession->next = NULL;
	//head->session_cnt += 1;

	return;
}

void addUser(struct user *head, struct user *myUser) {
	if (head == NULL) {
		printf("addUser: head is null\n");
		head = myUser;
		return;
	}
	struct user *ptr = head;
	while (ptr->next != NULL) {
		ptr = ptr->next;
	}

	ptr->next = myUser;
	myUser->next = NULL;
	//head->user_cnt = head->user_cnt + 1;// += 1;


	return;
}

struct session *findSession(struct session *head, char *name) {
	if (head == NULL) {
		printf("findSession: head is null\n");
		return NULL;
	}
	struct session *ptr = head;
	while (ptr != NULL) {
		if (strcmp(ptr->sessionName, name) == 0) {
			return ptr;
		}
		ptr = ptr->next;
	}
	printf("findSession: session not found\n");
	return NULL;
}

struct user *findUser(struct user *head, int fd) {
	if (head == NULL) {
		printf("findUser: head is null\n");
		return NULL;
	}
	struct user *ptr = head;
	while (ptr != NULL) {
		if (ptr->sockfd == fd) {
			return ptr;
		}
		ptr = ptr->next;
	}
	printf("findUser: user not found\n");
	return NULL;
}

void removeSession(struct session *head, char *sessName) {
	if (head == NULL) {
		printf("removeSession: head is null\n");
		return;
	}

	struct session *ptr = head;
	// if the session of interest is the head
	/*if (strcmp(ptr->sessionName, sessName) == 0) {
		//head->session_cnt -= 1;
		//head->next->session_cnt = head->session_cnt;
		head = head->next;
		free(ptr);
		return;
	}*/
	//ptr = ptr->next;
	struct session *pptr = NULL;
	while (ptr != NULL) {
		if (strcmp(pptr->sessionName, sessName) == 0) {
			if (pptr == NULL) {
			    head = head->next;
			    ptr->next = NULL;
			    free(ptr);
			    return;
			}
			pptr->next = ptr->next;
			ptr->next = NULL;
			free(pptr);
			return;
		}
		pptr = ptr;
		ptr = ptr->next;
	}
	//if session of interest is the last
	/*if (strcmp(ptr->sessionName, sessName) == 0) {
		head->session_cnt = 0;
		head->next = NULL;
		free(ptr);
		return;
	}*/
	printf("removeSession: session not found\n");
	return;
}

// prone to bugs
void removeUser(struct user *head, struct user *myUser) {
	if (head == NULL) {
		printf("removeUser: head is null\n");
		return;
	}
	//head->user_cnt -= 1;
	struct user *ptr = head;
	/*if (strcmp(ptr->name, myUser->name) == 0) {
		head->next->user_cnt = head->user_cnt;
		head = head->next;
		free(ptr);
		return;
	}*/

	struct user *pptr = NULL;
	while (ptr != NULL) {
		if (strcmp(ptr->name, myUser->name) == 0) {
			if (pptr == NULL) {
			    head = head->next;
			    ptr->next = NULL;
			    free(ptr);
			    return;
			}
			pptr->next = ptr->next;
			ptr->next = NULL;
			free(ptr);
			return;
		}
		pptr = ptr;
		ptr = ptr->next;
	}
	printf("removeUser: user not found / not logged in\n");
	return;
}

void removeSessUser(struct session *sessLeave, struct user *usr, struct session *head) {
	removeUser(sessLeave->users, usr);
	if (sessLeave->users == NULL) {
		removeSession(head, sessLeave->sessionName);
	}
	return;
}

/*void removeUser_fd(struct user *head, int fd) {
	if (head == NULL) {
		printf("removeUser: head is null\n");
		return;
	}
	head->user_cnt -= 1;
	struct user *ptr = head;
	if (ptr->sockfd == fd) {
		head->next->user_cnt = head->user_cnt;
		head = head->next;
		free(ptr);
		return;
	}

	struct user *pptr = head->next;
	while (pptr != NULL) {
		if (pptr->sockfd == fd) {
			ptr->next = pptr->next;
			pptr->next = NULL;
			free(pptr);
			return;
		}
		pptr = pptr->next;
		ptr = ptr->next;
	}
	printf("removeUser: user not found / not logged in\n");
	return;
}*/

void printUser(struct user *curr) {
	if (curr == NULL) {
		printf("NULL\n");
		return;
	}
	printf("username: %s\n", curr->name);
	printf("password: %s\n", curr->password); //for debug
	printf("sessionID: %s\n", curr->sessionID); //for debug
	printf("socket: %d\n", curr->sockfd); //for debug
	printf("user count: %d\n", curr->user_cnt);
}

void printUsers(struct user *head) {
	if (head == NULL) {
		printf("printUsers: head is null\n");
		return;
	}
	struct user *ptr = head;
	while (ptr != NULL) {
		printUser(ptr);
		ptr = ptr->next;
	}
	return;
}

void printSess(struct session *curr) {
	if (curr == NULL) {
		printf("NULL\n");
		return;
	}
	printf("session ID: %s\n", curr->sessionName);
	printf("session count %d\n", curr->session_cnt);
	printf("--------contianed Users----------\n");
	printUsers(curr->users);
}

void printSessions(struct session *head) {
	if (head == NULL) {
		printf("printSessions: head is null\n");
		return;
	}
	struct session *ptr = head;
	while (ptr != NULL) {
		printSess(ptr);
		ptr = ptr->next;
	}
	return;
}

// TODO Isamu
// TODO make sure that the next of the last element is always null!

// we start with a dummy session head and its dummy user linked list heads 
void sendToPeers(struct session *head, struct user *myUser, char *message, int sockfd){
	struct session *ptr = (struct session *)malloc(sizeof(struct session));
	struct session *nextSession = (struct session *)malloc(sizeof(struct session));
	struct user *myPtr = (struct user *)malloc(sizeof(struct user));
	struct user *nextUser = (struct user *)malloc(sizeof(struct user));
	int numbytes;

	ptr = head;
	printf("beginning sending to peers \n");

	//if we found the session
	if(strcmp(myUser->sessionID, ptr->next->sessionName) == 0){
		myPtr = ptr->next->users->next; //locate the users list
		
		while (myPtr != NULL){
			// send to each element of the list
			//if (myPtr->sockfd == sockfd) {
				//continue;
			//}
			if((numbytes = send(myPtr->sockfd, message, BUF_SIZE - 1, 0)) == -1){
				fprintf(stderr, "ACK error\n");
				//close(sockfd);
				return;
			}
			nextUser = myPtr->next;
			myPtr = nextUser;
		}

	} else{
		while(strcmp(myUser->sessionID, ptr->sessionName) != 0){
			nextSession = ptr->next;
			ptr = nextSession;
		} 

		myPtr = ptr->users->next;
		
		while (myPtr != NULL){
			// send to each element of the list
			
			if((numbytes = send(myPtr->sockfd, message, BUF_SIZE - 1, 0)) == -1){
				fprintf(stderr, "ACK error\n");
				close(sockfd);
				return;
			}
			nextUser = myPtr->next;
			myPtr = nextUser;
		}
	}
}

// in main we should make sure that the limit is not exceeded
// assuming that we found an empty session here.
void initSession(struct session *sessions, char *sessionID, struct user *newUser){
	struct session *ptr = sessions;
	struct user *myUser;

	ptr->sessionName = sessionID;
	myUser = ptr->users->next;
	myUser->next = newUser;

}


//note to self: function changes the content the pointer is pointing to
// packet to string
void formatMessage(struct message * myPacket, char *packetString) {
	memset(packetString, 0, BUF_SIZE);
	int strptr = sprintf(packetString, "%d:%d:%s:", myPacket->type, myPacket->size, myPacket->source);
	//printf("THE DATA %s", myPacket->data);
	memcpy(packetString + strptr, myPacket->data, myPacket->size);

	packetString[strlen(packetString)] = 0;
	//printf("message: %s\n", packetString);
	
	
}

void print_message(struct message * myPacket) {
	printf("type: %d\n", myPacket->type);
	printf("size: %d\n", myPacket->size);
	printf("source: %s\n", myPacket->source);
	printf("data: %s\n", myPacket->data);
}

// string to packet
struct message *formatString(char * buf) {
	if (strlen(buf) == 3) {
		printf("i'm here\n");
		return NULL;
	}
	struct message *packet_rcv = malloc(sizeof(struct message));
	char * myString[5];
	int i = 0;
	myString[0] = strtok(buf, ":");
	myString[1] = strtok(NULL, ":");
	myString[2] = strtok(NULL, ":");
	myString[3] = strtok(NULL, "\0");

	if (myString[3] == NULL) {
		printf("helloehelloe-------------\n");
		myString[3] = myString[2];
		myString[2] = NULL;
	}
	if (myString[0] != NULL) {
	    packet_rcv->type = atoi(myString[0]);
	}
	if (myString[1] != NULL) {
	    packet_rcv->size = atoi(myString[1]);
	}
	if (myString[2] != NULL) {
        strcpy(packet_rcv->source, myString[2]);
	}
	if (myString[3] != NULL) {
	    strcpy(packet_rcv->data, myString[3]);
	}
	//int strptr = strlen(myString[0]) + strlen(myString[1]) + strlen(myString[2]) + 3;
	//memcpy(packet_rcv->data, buf + strptr, packet_rcv->size);
	//printf("data: %s\n", packet_rcv->data);
	//print_message(packet_rcv);
	return packet_rcv;
}
 
