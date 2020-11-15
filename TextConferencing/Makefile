TARGET = server client
all: ${TARGET}

server: server.c message.h 
	gcc -o server server.c

client: client.c message.h
	gcc -o client client.c

clean:
	rm -f ${TARGET}
