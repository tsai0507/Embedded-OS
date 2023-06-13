CC = gcc
all: hw2

hw2: hw2.c sockop.o
	$(CC) -o hw2 hw2.c sockop.o

sockop.o: sockop.c
	$(CC) -c sockop.c

clean:
	rm hw2 *o 