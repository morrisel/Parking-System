# Source directory
SRC_DIR = ../src

# Source files
IPC_SENDER_C = $(SRC_DIR)/ipc_sender.c
TCP_CLIENT_C = $(SRC_DIR)/tcp_client.c

# Object files
IPC_SENDER_O = ipc_sender.o
TCP_CLIENT_O = tcp_client.o

# Output executables
OUT_IPC_SENDER = out_ipc_sender
OUT_TCP_CLIENT = out_tcp_client

# Compiler and flags
CC = gcc
CFLAGS = -c
LDFLAGS = 

# Default target
all: $(OUT_IPC_SENDER) $(OUT_TCP_CLIENT)

# Step 1: Compilation of source files into object files
$(IPC_SENDER_O): $(IPC_SENDER_C)
	$(CC) $(CFLAGS) $< -o $@

$(TCP_CLIENT_O): $(TCP_CLIENT_C)
	$(CC) $(CFLAGS) $< -o $@

# Step 2: Linking object files into executables
$(OUT_IPC_SENDER): $(IPC_SENDER_O)
	$(CC) $^ -o $@

$(OUT_TCP_CLIENT): $(TCP_CLIENT_O)
	$(CC) $^ -o $@


## Install links in ~/bin directory
#.PHONY: install
#install: $(OUT_IPC_SENDER) $(OUT_TCP_CLIENT)
#	ln -sf $(CURDIR)/$(OUT_IPC_SENDER) ~/bin/$(OUT_IPC_SENDER)
#	ln -sf $(CURDIR)/$(OUT_TCP_CLIENT) ~/bin/$(OUT_TCP_CLIENT)
#



# Install links in specified directory
.PHONY: install
install: $(OUT_IPC_SENDER) $(OUT_TCP_CLIENT)
ifndef TARGET_DIR
	$(error TARGET_DIR is not set. Use 'make install TARGET_DIR=../../bin')
endif
	ln -sf $(CURDIR)/$(OUT_IPC_SENDER) $(TARGET_DIR)/$(OUT_IPC_SENDER)
	ln -sf $(CURDIR)/$(OUT_TCP_CLIENT) $(TARGET_DIR)/$(OUT_TCP_CLIENT)



# Cleaning compiled files
.PHONY: clean
clean:
	rm -f $(IPC_SENDER_O) $(OUT_IPC_SENDER)
	rm -f $(TCP_CLIENT_O) $(OUT_TCP_CLIENT)


