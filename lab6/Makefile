CC = gcc
all: client server

client: client.c sockop.o
	$(CC) -o client client.c sockop.o

server: server.c sockop.o
	$(CC) -o server server.c sockop.o -lpthread

sockop.o: sockop.c
	$(CC) -c sockop.c

clean:
	rm server *o client