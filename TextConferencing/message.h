#define MAX_NAME 1100
#define MAX_DATA 4096

struct message {
    unsigned int type;
    unsigned int size;
    unsigned char source[MAX_NAME];
    unsigned char data[MAX_DATA];
};