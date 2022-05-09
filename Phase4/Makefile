CFLAGS = -O
CC = g++

all: Client Server dummyProgram

Client: Phase4-client.c
	gcc -o Phase4-client.o Phase4-client.c 

Server: Phase4-server.c
	gcc -o Phase4-server.o Phase4-server.c -lpthread

dummyProgram: dummyProgram.c
	gcc -o dummyProgram.o dummyProgram.c -lpthread -lrt 

clean: 
	rm -f core *.o 
