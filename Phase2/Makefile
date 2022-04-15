CFLAGS = -O
CC = g++

all: Client Server

Client: Phase2-client.c
	gcc -o Phase2-client.o Phase2-client.c

Server: Phase2-server.c
	gcc -o Phase2-server.o Phase2-server.c

clean: 
	rm -f core *.o 
