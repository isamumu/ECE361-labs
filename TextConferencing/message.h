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

char *formatMessage(struct message * myPacket, char *packetString) {
	memset(packetString, 0, BUF_SIZE);
	int strptr = sprintf(packetString, "%d:%d:%s:", myPacket->type, myPacket->size, myPacket->source);
	memcpy(packetString + strptr, myPacket->data, myPacket->size);
	return packetString;
	
}

void print_message(struct message * myPacket) {
	printf("total_frag: %d\n", myPacket->type);
	printf("frag_no: %d\n", myPacket->size);
	printf("size: %d\n", myPacket->source);
	printf("filename: %s\n", myPacket->data);
}


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
 
