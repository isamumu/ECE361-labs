#define MAX_NAME 1100
#define MAX_DATA 4096
#define BUF_SIZE 1100

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
    char name[MAX_NAME];
    char password[MAX_NAME];
    char *sessionID;
    int sockfd; // used as ref to send to every user in the linked list
    struct user *next;// = NULL; // NOTE: init to null here
    int user_cnt;// = -1; //change on the dummy to 0
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

// TODO Hannah
void addSession(struct session *head, struct session *mySession) {
	if (head == NULL) {
		printf("addSession: head is null\n");
		return;
	}
	struct session *ptr = head;
	while (ptr->next != NULL) {
		ptr = ptr->next;
	}

	mySession->users->user_cnt = 0;

	ptr->next = mySession;
	mySession->next = NULL;
	head->session_cnt += 1;

	return;
}

void addUser(struct user *head, struct user *myUser) {
	if (head == NULL) {
		printf("addUser: head is null\n");
		return;
	}
	struct user *ptr = head;
	while (ptr->next != NULL) {
		ptr = ptr->next;
	}

	ptr->next = myUser;
	myUser->next = NULL;
	head->user_cnt += 1;


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
	printf("findSession: session not found\n");
	return NULL;
}

// prone to bugs
void removeSession(struct session *head, char *sessName) {
	if (head == NULL) {
		printf("removeSession: head is null\n");
		return;
	}

	struct session *ptr = head;
	// if the session of interest is the head
	if (strcmp(ptr->sessionName, sessName) == 0) {
		head->session_cnt -= 1;
		head->next->session_cnt = head->session_cnt;
		head = head->next;
		free(ptr);
		return;
	}

	struct session *pptr = head->next;
	while (pptr != NULL) {
		if (strcmp(pptr->sessionName, sessName) == 0) {
			ptr->next = pptr->next;
			pptr->next = NULL;
			free(pptr);
			return;
		}
		pptr = pptr->next;
		ptr = ptr->next;
	}
	printf("removeSession: session not found\n");
	return;
}

// prone to bugs
void removeUser(struct user *head, struct user *myUser) {
	if (head == NULL) {
		printf("removeUser: head is null\n");
		return;
	}
	head->user_cnt -= 1;
	struct user *ptr = head;
	if (strcmp(ptr->name, myUser->name) == 0) {
		head->next->user_cnt = head->user_cnt;
		head = head->next;
		free(ptr);
		return;
	}

	struct user *pptr = head->next;
	while (pptr != NULL) {
		if (strcmp(pptr->name, myUser->name) == 0) {
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
}

void printUser(struct user *curr) {
	if (curr == NULL) {
		printf("NULL\n");
		return;
	}
	printf("username: %s\n", curr->name);
	printf("password: %%s\n", curr->password); //for debug
	printf("sessionID: %s\n", curr->sessionID); //for debug
	printf("socket: %d\n", curr->sockfd); //for debug
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
	printf("contianed Users:\n");
	printf("---------------------\n");
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
	struct session *ptr = head;
	struct session *nextSession;
	struct user *myPtr, *nextUser;
	int numbytes;

	if(strcmp(myUser->sessionID, ptr->sessionName) == 0){
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
	memcpy(packetString + strptr, myPacket->data, myPacket->size);
	
	
}

void print_message(struct message * myPacket) {
	printf("total_frag: %d\n", myPacket->type);
	printf("frag_no: %d\n", myPacket->size);
	printf("size: %d\n", myPacket->source);
	printf("filename: %s\n", myPacket->data);
}

// string to packet
struct message *formatString(char * buf) {
	struct message *packet_rcv = malloc(sizeof(struct message));
	char * myString[5];
	int i = 0;
	myString[0] = strtok(buf, ":");
	myString[1] = strtok(NULL, ":");
	myString[2] = strtok(NULL, ":");
	//myString[3] = strtok(NULL, ":");
	packet_rcv->type = atoi(myString[0]);
	packet_rcv->size = atoi(myString[1]);
        strcpy(packet_rcv->source, myString[2]);
	int strptr = strlen(myString[0]) + strlen(myString[1]) + strlen(myString[2]) + 3;
	memcpy(packet_rcv->data, buf + strptr, packet_rcv->size);
	print_message(packet_rcv);
	return packet_rcv;
}
 
