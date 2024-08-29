CC = gcc
CFLAGS = -Wall

all: main

main: main.o gpio.o uart.o
	$(CC) $(CFLAGS) -o main main.o gpio.o uart.o

main.o: main.c gpio.h uart.h
	$(CC) $(CFLAGS) -c main.c

gpio.o: gpio.c gpio.h
	$(CC) $(CFLAGS) -c gpio.c

uart.o: uart.c uart.h
	$(CC) $(CFLAGS) -c uart.c

clean:
	rm -f *.o main

