#define ACK "ACK"
#define UNACK "UNACK"

struct packet {
	unsigned int total_frag;
	usigned int frag_no;
	unsigned int size;
	char* filename;
	char filedata[1000];
}
