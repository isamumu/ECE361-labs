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
	struct user *next;
	int user_cnt = 0; //change on the dummy

};

// for each session created
struct session {
	char *sessionName;
	//int session_number;
	//int socketfds[10];
	//int user_cnt = 0; //change on the dummy
	int session_cnt = 0; //change on first node
	struct user *users;
	struct session *next;
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

	ptr->next = mySession;
	mySession->next = NULL;
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
	return;
}

struct session *findSession(struct session *head, char *name) {
	if (head == NULL) {
		printf("findSession: head is null\n");
		return NULL;
	}
	struct session *ptr = head;
	while (ptr != NULL) {
		if (strcmp(pptr->sessionName, name) == 0) {
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
	struct session *ptr = head;
	while (ptr != NULL) {
		if (strcmp(pptr->sockfd, fd) == 0) {
			return ptr;
		}
		ptr = ptr->next;
	}
	printf("findSession: session not found\n");
	return NULL;
}

void removeSession(struct session *head, struct session *mySession) {
	if (head == NULL) {
		printf("removeSession: head is null\n");
		return;
	}
	struct session *ptr = head;
	if (strcmp(ptr->sessionName, mySession->sessionName) == 0) {
		head->next->session_cnt = head->session_cnt;
		head = head->next;
		free(ptr);
		return;
	}

	struct session *pptr = head->next;
	while (pptr != NULL) {
		if (strcmp(pptr->sessionName, mySession->sessionName) == 0) {
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
void removeUser(struct user *head, struct user *myUser) {
	if (head == NULL) {
		printf("removeUser: head is null\n");
		return;
	}
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
void sendToSession(struct user *users);
void initSession(struct session *sessions);


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
	myString[3] = strtok(NULL, ":");
	packet_rcv->type = atoi(myString[0]);
	packet_rcv->size = atoi(myString[1]);
    packet_rcv->source = myString[2];
	int strptr = strlen(myString[0]) + strlen(myString[1]) + strlen(myString[2]) + 3;
	memcpy(packet_rcv->data, buf + strptr, packet_rcv->size);
	print_packet(packet_rcv);
	return packet_rcv;
}
 
