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
	unsigned int frag_no; //sequence number starting from 1
	unsigned int size; //set to size of data (0, 1000)
	char* filename; 
	char filedata[1000]; //binary
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

/*
char *formatPacket(struct packet*){
	char* packetString;
	
	return packetString;

}*/

char *formatPacket(struct packet * myPacket, char *packetString) {
	int strptr = sprintf(packetString, "%d:%d:%d:%s:", myPacket->total_frag, myPacket->frag_no, myPacket->size, myPacket->filename);
	memcpy(packetString + strptr, myPacket->filedata, myPacket->size);
	return packetString;
	
}

void print_packet(struct packet * myPacket) {
	printf("total_frag: %d\n", myPacket->total_frag);
	printf("frag_no: %d\n", myPacket->frag_no);
	printf("size: %d\n", myPacket->size);
	printf("filename: %s\n", myPacket->filename);
	printf("filedata: %s\n", myPacket->filedata);
}


struct packet *formatString(char * buf) {
	printf("i'm here?\n");
	struct packet *packet_rcv = malloc(sizeof(struct packet));
	char * myString[5];
	int i = 0;
	char * temp = strtok(buf, ":"); //temp variable to store the sections of string
	//int strptr = strlen(temp); //ptr to the buf
	//packet_rcv->total_frag = atoi(temp);
	//printf("total_frag: %d\n", packet_rcv->total_frag);
	while (temp != NULL) {
		myString[i] = temp;
		temp = strtok(NULL, ":");
		i++;
	}
	packet_rcv->total_frag = atoi(myString[0]);
	packet_rcv->frag_no = atoi(myString[1]);
        packet_rcv->size = atoi(myString[2]);
	packet_rcv->filename = myString[3];
	memcpy(packet_rcv->filedata, myString[4], packet_rcv->size);
	/*temp = strtok(buf + strptr, ":");
	packet_rcv->frag_no = atoi(temp);
	strptr += strlen(temp);
	printf("frag_no: %d\n", packet_rcv->frag_no);

	temp = strtok(buf + strptr, ":");
	packet_rcv->size = atoi(temp);
	strptr += strlen(temp);

	temp = strtok(buf + strptr, ":");
	packet_rcv->filename = temp;
	strptr += strlen(temp);

	memcpy(packet_rcv->filedata, &buf[strptr], packet_rcv->size);
	*/
	print_packet(packet_rcv);
	return packet_rcv;
			
}
*/


char** fragment_this(char* filename, int * fragNum){ //char *
	
	FILE *fp; // need a file pointer to open a file. 
	fp = fopen(filename, "r");

	// see if we require fragmenting
	//int numFrags;
	int fileSize = getBytes(filename);

	*fragNum = (fileSize/BYTE_LIMIT) + 1; // plus 1 bc we have at least 1 frag	
	int numFrags = *fragNum;
	char **packets = malloc(sizeof(char*) * numFrags); // to store packets

	// Q: why is it a char pointer?
	// A: because, each pointer to a packet is technically a pointer to a char pointer
	// remember: in C, a string is basically a char pointer to the first character

	for(int packNo = 0; packNo < numFrags; packNo++){ // run at least once
		printf("Creating packNo %d\n", packNo + 1);
		struct packet paketto;	
		memset(paketto.filedata, 0, sizeof(char) * (BYTE_LIMIT));

		printf("i'm here\n");
		FILE* fp = fopen(filename, "r"); 
		// order: total_frag, frag_no, size, filename, filedata
		paketto.total_frag = numFrags;
		paketto.frag_no = packNo + 1;
		paketto.filename = filename;
		fread(paketto.filedata, sizeof(char), BYTE_LIMIT, fp);
		
		if((packNo+1) != numFrags){
			paketto.size = BYTE_LIMIT;

		} else {
			printf("i'm here too\n");
			fseek(fp, 0, SEEK_END); 
			// calculating the size of the file 
			paketto.size = (ftell(fp) - 1) % 1000 + 1; // off by 1 corner case
		}
		print_packet(&paketto);

		// bc the array stores char pointers, we must allocate space for each packet string
		printf("mallocing space from packNo %d\n", packNo + 1);
		packets[packNo] = malloc(BYTE_LIMIT * sizeof(char)); 
		
		printf("Converting packNo %d\n", packNo + 1);
		// TODO: store the packet here
		formatPacket(&paketto, packets[packNo]);
		//packets[packNo] = formatPacket(&paketto);	

	}

	return packets;

}

void free_fragments(char** packets, int numFrag) {
	for (int i = 0; i < numFrag; i++) {
		free(packets[i]);
	}
	free(packets);
	packets = NULL;
}
