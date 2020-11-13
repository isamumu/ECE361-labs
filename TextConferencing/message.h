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
    char sessionID[MAX_NAME];
    int sockfd; // used as ref to send to every user in the linked list
	struct user *next;
	int user_cnt = 0; //change on the dummy

};

// for each session created
struct session {
	char *sesionName;
	int session_number;
	//int socketfds[10];
	int fd_count = 0; //change on the dummy
	struct user *users;
	struct session *next;
}

// TODO Hannah
void addSession(struct session *sessions, struct session *mySession);
void addUser(struct user *users, struct user *myUser);

void removeSession(struct session *sessions, struct session *mySession);
void removeUser(struct user *users, struct user *myUser);

void printSessions(struct session *sessions);
void printUsers(struct user *users);

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
 
