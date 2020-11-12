#define MAX_NAME 1100
#define MAX_DATA 4096
#define BUF_SIZE 1100

// because type is an unsigned integer
enum type {
    LOGIN,
    LO_ACK,
    LO_NAK,
    EXIT,
    JOIN,
    JN_ACK,
    JN_NAK,
    LEAVE_SESS,
    NEW_SESS,
    NS_ACK,
    MESSAGE,
    QUERY,
    QU_ACK
};

struct message {
    unsigned int type; 
    unsigned int size;
    unsigned char source[MAX_NAME];
    unsigned char data[MAX_DATA];
};

// function to convert: packet --> string

char *formatPacket(struct message *myMessage, char *packetString) {
	memset(packetString, 0, BUF_SIZE);
	int strptr = sprintf(packetString, "%d:%d:%s:", myMessage->type, myMessage->size, myMessage->source);
	memcpy(packetString + strptr, myPacket->data, myMessage->size);
	return packetString;
}

// function to convert: string --> packet

struct message *formatString(char * buf) {
	struct message *message_rcv = malloc(sizeof(struct message));
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
