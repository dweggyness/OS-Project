CFLAGS = -O
CC = g++

all: Client Server Test

Client: Phase3-client.c
	gcc -o Phase3-client.o Phase3-client.c 

Server: Phase3-server.c
	gcc -o Phase3-server.o Phase3-server.c -lpthread

Test: Test.c
	gcc -o Test.o Test.c

clean: 
	rm -f core *.o 
