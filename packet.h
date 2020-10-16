#define BYTE_LIMIT 1000
#define BUF_SIZE 1024

///////////////////////////////////////////////////////////////////////////////////
/// The total_frag field indicates the total number of fragments of the file.   ///
/// Each packet contains one fragment. The frag_no field indicates the sequence ///
/// number of the fragment, starting from 1. The size field should be set to    ///
/// the size of the data. Therefore, it should be in the range of 0 to 1000.    ///
/// All members of the packet should be sent as a single string, each field     ///
/// separated by a colon.                                                       ///
///////////////////////////////////////////////////////////////////////////////////

// SAMPLE: packet = “3:2:10:foobar.txt:lo World!\n”
// *note: the beginning of the packet is just text, but the rest is binary, 
//        so don't use string mans!
// TEST FOR TEXT AND IMAGE FILES
// HINT: implement a simple stop-and-wait ACK to ensure correct receipts
// SERVER: use ACK and NACK to control data flow from sender
// CLIENT: open a UDP socket to listen for ACKS

struct packet {
	unsigned int total_frag;
	unsigned int frag_no;
	unsigned int size;
	char* filename;
	char filedata[1000];
};

int getBytes(char* filename){
	// opening the file in read mode 
    FILE* fp = fopen(filename, "r"); 
  
    if (fp == NULL) { 
        printf("File Not Found!\n"); 
        return -1; 
    } 
  
    fseek(fp, 0L, SEEK_END); 
  
    // calculating the size of the file 
    int num = ftell(fp); 
  
    fclose(fp); 
  
    return num;

}

char *formatPacket(struct packet*){
	char* packetString;

	return packetString;

}

struct packet* fragment_this(char* filename){
	
	FILE *fp; // need a file pointer to open a file. 
	fp = fopen(filename "r");

	// see if we require fragmenting
	int numFrags;
	int fileSize = getBytes(filename);

	numFrags = (fileSize/BYTE_LIMIT) + 1; // plus 1 bc we have at least 1 frag
	char *packets[numFrags] = malloc(sizeof(char*) * numFrags); // to store packets

	// Q: why is it a char pointer?
	// A: because, each pointer to a packet is technically a pointer to a char pointer
	// remember: in C, a string is basically a char pointer to the first character

	for(int packNo = 0; packNo < numFrags; packNo++){ // run at least once
		struct packet paketto;	
		memset(packet.filedata, 0, sizeof(char) * (BYTE_LIMIT));

		FILE* fp = fopen(filename, "r"); 
		// order: total_frag, frag_no, size, filename, filedata
		paketto.total_frag = numFrags;
		paketto.frag_no = packNo + 1;
		
		if((packNo+1) != numFrags){
			packet.size = BYTE_LIMIT;

		} else {
			fseek(fp, 0L, SEEK_END); 
			// calculating the size of the file 
			paketto.size = (ftell(fp) - 1) % 1000 + 1; // off by 1 corner case
		}

		// bc the array stores char pointers, we must allocate space for each packet string
		packets[packNo] = malloc(BYTE_LIMIT * sizeof(char)); 

		// TODO: store the packet here
		// ... 
		// ...

	}


}