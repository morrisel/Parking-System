# This is Makefile for constructing the test version

# Compiler and flags
CC = gcc
#CFLAGS = -Wall -Wextra -O2 -I./gpio -I./uart
CFLAGS = -Wall -I../core/inc -I../drivers/gpio -I../drivers/uart

# Source directories
CORE_SRC_DIR = ../core/src
CORE_INC_DIR = ../core/inc
DRIVERS_SRC_DIR = ../drivers

# Object directories
OBJ_DIR_DEBUG = ./debug
OBJ_DIR_CORE = ./debug/core
OBJ_DIR_DRIVERS = ./debug/drivers

TARGET_DIR = ../../bin

# Output executables
IPC_SENDER = out_ipc_sender
TCP_CLIENT = out_tcp_client
SYS_COM_CONTROLLER = sys_com_controller


# Default Goals
all: $(IPC_SENDER) $(TCP_CLIENT) $(SYS_COM_CONTROLLER)


# Rules for creating executables
# ------------------------------
$(IPC_SENDER): $(OBJ_DIR_CORE)/ipc_sender.o
	$(CC) $(CFLAGS) -o $(IPC_SENDER) $< -lrt

$(TCP_CLIENT): $(OBJ_DIR_CORE)/tcp_client.o
	$(CC) $(CFLAGS) -o $(TCP_CLIENT) $<

$(SYS_COM_CONTROLLER): $(OBJ_DIR_CORE)/sys_com_controller.o $(OBJ_DIR_CORE)/data_formatter.o \
	$(OBJ_DIR_DRIVERS)/gpio.o $(OBJ_DIR_DRIVERS)/uart.o
	$(CC) $(CFLAGS) -o $(SYS_COM_CONTROLLER) $^


# Rules for compilations
# ----------------------
$(OBJ_DIR_CORE)/data_formatter.o: $(CORE_SRC_DIR)/data_formatter.c $(CORE_INC_DIR)/data_formatter.h
	@mkdir -p $(OBJ_DIR_CORE)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR_CORE)/ipc_sender.o: $(CORE_SRC_DIR)/ipc_sender.c
	@mkdir -p $(OBJ_DIR_CORE)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR_CORE)/tcp_client.o: $(CORE_SRC_DIR)/tcp_client.c
	@mkdir -p $(OBJ_DIR_CORE)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR_CORE)/sys_com_controller.o: $(CORE_SRC_DIR)/sys_com_controller.c \
	$(DRIVERS_SRC_DIR)/gpio/gpio.h $(DRIVERS_SRC_DIR)/uart/uart.h \
	$(CORE_INC_DIR)/data_formatter.h
	@mkdir -p $(OBJ_DIR_CORE)
	$(CC) $(CFLAGS) -c $< -o $@


$(OBJ_DIR_DRIVERS)/gpio.o: $(DRIVERS_SRC_DIR)/gpio/gpio.c $(DRIVERS_SRC_DIR)/gpio/gpio.h 
	@mkdir -p $(OBJ_DIR_DRIVERS)
	$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJ_DIR_DRIVERS)/uart.o: $(DRIVERS_SRC_DIR)/uart/uart.c $(DRIVERS_SRC_DIR)/uart/uart.h 
	@mkdir -p $(OBJ_DIR_DRIVERS)
	$(CC) $(CFLAGS) -c $< -o $@
	

# Install symbolic links in bin directory
.PHONY: install
install: $(IPC_SENDER) $(TCP_CLIENT) $(SYS_COM_CONTROLLER)
ifndef TARGET_DIR
	$(error TARGET_DIR is not set. Use 'make install TARGET_DIR=../../bin')
endif
	@echo "Setting up soft links in bin directory"
	@echo "Linking $(IPC_SENDER) and $(TCP_CLIENT) to $(TARGET_DIR)"
	ln -sf $(CURDIR)/$(IPC_SENDER) $(TARGET_DIR)/$(IPC_SENDER)
	ln -sf $(CURDIR)/$(TCP_CLIENT) $(TARGET_DIR)/$(TCP_CLIENT)
	ln -sf $(CURDIR)/$(SYS_COM_CONTROLLER) $(TARGET_DIR)/$(SYS_COM_CONTROLLER)


# Clearing intermediate files
.PHONY: clean
clean:
	rm -f $(OBJ_DIR_CORE)/*.o $(OBJ_DIR_DRIVERS)/*.o $(IPC_SENDER) $(TCP_CLIENT) $(SYS_COM_CONTROLLER)
	rmdir --ignore-fail-on-non-empty $(OBJ_DIR_CORE) $(OBJ_DIR_DRIVERS) $(OBJ_DIR_DEBUG)
	rm -f $(TARGET_DIR)/out_* $(TARGET_DIR)/$(SYS_COM_CONTROLLER)

