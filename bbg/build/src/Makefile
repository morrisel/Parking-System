# This is Makefile for constructing the test version

CC = gcc
#CFLAGS = -Wall -Wextra -O2
CFLAGS = -Wall -I./gpio -I./uart

# Executables
IPC_SENDER = out_ipc_sender
TCP_CLIENT = out_tcp_client

# Default Goals
all:	$(IPC_SENDER) $(TCP_CLIENT) main


# Rules for creating executables
# ---------------------------------------------------------
$(IPC_SENDER): ipc_sender.o
	$(CC) $(CFLAGS) -o $(IPC_SENDER) ipc_sender.o -lrt

$(TCP_CLIENT): tcp_client.o
	$(CC) $(CFLAGS) -o $(TCP_CLIENT) tcp_client.o

main: main.o gpio.o uart.o data_formatter.o
	$(CC) $(CFLAGS) -o main main.o gpio.o uart.o data_formatter.o


# Rules for compilations
# ---------------------------------------------------------
data_formatter.o: data_formatter.c data_formatter.h
	$(CC) $(CFLAGS) -c data_formatter.c

ipc_sender.o: ipc_sender.c
	$(CC) $(CFLAGS) -c ipc_sender.c

tcp_client.o: tcp_client.c
	$(CC) $(CFLAGS) -c tcp_client.c

main.o: main.c gpio/gpio.h uart/uart.h data_formatter.h
	$(CC) $(CFLAGS) -c main.c

gpio.o: gpio/gpio.c gpio/gpio.h
	$(CC) $(CFLAGS) -c gpio/gpio.c

uart.o: uart/uart.c uart/uart.h
	$(CC) $(CFLAGS) -c uart/uart.c



# Clearing intermediate files and ext_data_ws
clean:
	rm -f *.o $(IPC_SENDER) $(TCP_CLIENT) main

