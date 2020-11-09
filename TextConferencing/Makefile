TARGET = server deliver
all: ${TARGET}

server: server.c packet.h 
	gcc -o server server.c

client: client.c packet.h
	gcc -o client client.c

clean:
	rm -f ${TARGET}
