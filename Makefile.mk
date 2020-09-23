
TARGET = server deliverer
all: $(TARGET)

server: server.c 
    gcc -g -Wall -o server 

deliverer: deliverer.c 
    gcc -g -Wall deliverer

clean:
    rm -f ${TARGET}