# This is Makefile for constructing the Server station
# ----------------------------------------------------

# Compiler and flags
CC = gcc
#CFLAGS = -Wall -Wextra -O2 -I./gpio -I./uart
#CFLAGS = -Wall -I../core/inc -I../drivers/gpio -I../drivers/uart
CFLAGS = -Wall -I../core/inc

# Source directories
CORE_SRC_DIR = ../core/src
CORE_INC_DIR = ../core/inc

# Object directories
OBJ_DIR_DEBUG = ./debug
OBJ_DIR_CORE = ./debug/core

TARGET_DIR = ../../bin


# Output executables
SERVER = out_server
LISTENER = out_listener
GIIS = out_giis
INSERT_DATA_FROM_GIIS_SHM = out_insert_data_from_giis_shm
UPDATE_PRICES = out_update_prices
PRK_SYS_SRV_RUN = prk_sys_srv_run


# Default goals
all: $(SERVER) $(LISTENER) $(GIIS) $(INSERT_DATA_FROM_GIIS_SHM) $(UPDATE_PRICES) $(PRK_SYS_SRV_RUN)


# Rules for creating executables
# ------------------------------
$(SERVER): $(OBJ_DIR_CORE)/server.o
	$(CC) $(CFLAGS) -o $(SERVER) $< -lpthread

$(LISTENER): $(OBJ_DIR_CORE)/listener.o
	$(CC) $(CFLAGS) -o $(LISTENER) $<

$(GIIS): $(OBJ_DIR_CORE)/giis.o
	$(CC) $(CFLAGS) -o $(GIIS) $<  -lpthread

$(INSERT_DATA_FROM_GIIS_SHM): $(OBJ_DIR_CORE)/insert_data_from_giis_shm.o
	$(CC) $(CFLAGS) -o $(INSERT_DATA_FROM_GIIS_SHM) $<

$(UPDATE_PRICES): $(OBJ_DIR_CORE)/update_prices.o
	$(CC) $(CFLAGS) -o $(UPDATE_PRICES) $<

$(PRK_SYS_SRV_RUN): $(OBJ_DIR_CORE)/prk_sys_srv_run.o
	$(CC) $(CFLAGS) -o $(PRK_SYS_SRV_RUN) $<


# Rules for compilations
# ----------------------
$(OBJ_DIR_CORE)/server.o: $(CORE_SRC_DIR)/server.c
	@mkdir -p $(OBJ_DIR_CORE)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR_CORE)/listener.o: $(CORE_SRC_DIR)/listener.c
	@mkdir -p $(OBJ_DIR_CORE)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR_CORE)/giis.o: $(CORE_SRC_DIR)/giis.c
	@mkdir -p $(OBJ_DIR_CORE)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR_CORE)/insert_data_from_giis_shm.o: $(CORE_SRC_DIR)/insert_data_from_giis_shm.c
	@mkdir -p $(OBJ_DIR_CORE)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR_CORE)/update_prices.o: $(CORE_SRC_DIR)/update_prices.c
	@mkdir -p $(OBJ_DIR_CORE)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR_CORE)/prk_sys_srv_run.o: $(CORE_SRC_DIR)/prk_sys_srv_run.c
	@mkdir -p $(OBJ_DIR_CORE)
	$(CC) $(CFLAGS) -c $< -o $@


# Install symbolic links in bin directory
.PHONY: install
install: $(SERVER) $(LISTENER) $(GIIS) $(INSERT_DATA_FROM_GIIS_SHM) $(UPDATE_PRICES) $(PRK_SYS_SRV_RUN)
ifndef TARGET_DIR
	$(error TARGET_DIR is not set. Use 'make install TARGET_DIR=../../bin')
endif
	@echo "Linking out_* in bin directory"
	ln -sf $(CURDIR)/$(SERVER)                      $(TARGET_DIR)/$(SERVER)
	ln -sf $(CURDIR)/$(LISTENER)                    $(TARGET_DIR)/$(LISTENER)
	ln -sf $(CURDIR)/$(GIIS)                        $(TARGET_DIR)/$(GIIS)
	ln -sf $(CURDIR)/$(INSERT_DATA_FROM_GIIS_SHM)   $(TARGET_DIR)/$(INSERT_DATA_FROM_GIIS_SHM)
	ln -sf $(CURDIR)/$(UPDATE_PRICES)               $(TARGET_DIR)/$(UPDATE_PRICES)
	ln -sf $(CURDIR)/$(PRK_SYS_SRV_RUN)                 $(TARGET_DIR)/$(PRK_SYS_SRV_RUN)


# Clearing intermediate files
.PHONY: clean
clean:
	rm -f $(OBJ_DIR_CORE)/*.o $(SERVER) $(LISTENER) $(GIIS) $(INSERT_DATA_FROM_GIIS_SHM) $(UPDATE_PRICES) $(PRK_SYS_SRV_RUN)
	rmdir --ignore-fail-on-non-empty $(OBJ_DIR_CORE) $(OBJ_DIR_DEBUG)
	@echo "Remove links from bin directory:"
	rm -f $(TARGET_DIR)/$(SERVER)
	rm -f $(TARGET_DIR)/$(LISTENER)
	rm -f $(TARGET_DIR)/$(GIIS)
	rm -f $(TARGET_DIR)/$(INSERT_DATA_FROM_GIIS_SHM)
	rm -f $(TARGET_DIR)/$(UPDATE_PRICES)
	rm -f $(TARGET_DIR)/$(PRK_SYS_SRV_RUN)


