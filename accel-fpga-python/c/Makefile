CC=gcc
CFLAGS= -c -Wall
all: main
main: main.o accel.o
	$(CC) main.o accel.o -o main
main.o: main.c
	$(CC) $(CFLAGS) main.c
accel.o: accel.c accel.h
	$(CC) $(CFLAGS) accel.c
clean:
	rm -rf *.o 

