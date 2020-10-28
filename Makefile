TARGET = server deliver
all: ${TARGET}

server: server.c packet.h 
	gcc -o server server.c

deliver: deliver.c packet.h
	gcc -o deliver deliver.c

clean:
	rm -f ${TARGET}
