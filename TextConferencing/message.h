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


    
	myString[0] = strtok(buf, ":");
	myString[1] = strtok(NULL, ":");
	myString[2] = strtok(NULL, ":");
	myString[3] = strtok(NULL, ":");
 
