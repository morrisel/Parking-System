# Path to the vgps directory
VGPS_DIR = vgps

# File names
GPS_CREATOR_C      = $(VGPS_DIR)/gps_creator.c
GPS_CREATOR_O      = $(VGPS_DIR)/gps_creator.o
GPS_FUNCTIONS_C    = $(VGPS_DIR)/gps_functions.c
GPS_FUNCTIONS_O    = $(VGPS_DIR)/gps_functions.o
OUT_GPS_CREATOR    = $(VGPS_DIR)/out_gps_creator

# New directories and files for ext_data project
EXT_DATA_DIR       = .
SRC_DIR            = $(EXT_DATA_DIR)/src
INCLUDE_DIR        = $(EXT_DATA_DIR)/include

# Explicitly listed source and object files
CONFIG_C           = $(SRC_DIR)/config.c
CONFIG_O           = $(SRC_DIR)/config.o
FILE_OPERATIONS_C  = $(SRC_DIR)/file_operations.c
FILE_OPERATIONS_O  = $(SRC_DIR)/file_operations.o
GPS_GET_DATACAR_C  = $(SRC_DIR)/gps_get_datacar.c
GPS_GET_DATACAR_O  = $(SRC_DIR)/gps_get_datacar.o

OUT_GPS_DATACAR    = $(EXT_DATA_DIR)/gps_get_datacar

# Compiler and flags
CC                 = gcc
CFLAGS             = -c -I$(VGPS_DIR)
EXT_CFLAGS         = -c -I$(INCLUDE_DIR)

# Default target
all: $(OUT_GPS_CREATOR) $(OUT_GPS_DATACAR)

# Step 1: Compilation of source files into object files for vgps
$(GPS_CREATOR_O): $(GPS_CREATOR_C)
	$(CC) $(CFLAGS) $< -o $@

$(GPS_FUNCTIONS_O): $(GPS_FUNCTIONS_C)
	$(CC) $(CFLAGS) $< -o $@

# Step 2: Compilation and linking of the final executable for vgps
$(OUT_GPS_CREATOR): $(GPS_CREATOR_O) $(GPS_FUNCTIONS_O)
	$(CC) $^ -o $@

# Step 1: Compilation of source files into object files for ext_data
$(CONFIG_O): $(CONFIG_C)
	$(CC) $(EXT_CFLAGS) $< -o $@

$(FILE_OPERATIONS_O): $(FILE_OPERATIONS_C)
	$(CC) $(EXT_CFLAGS) $< -o $@

$(GPS_GET_DATACAR_O): $(GPS_GET_DATACAR_C)
	$(CC) $(EXT_CFLAGS) $< -o $@

# Step 2: Compilation and linking of the final executable for ext_data
$(OUT_GPS_DATACAR): $(CONFIG_O) $(FILE_OPERATIONS_O) $(GPS_GET_DATACAR_O)
	$(CC) $^ -o $@

# Cleaning compiled files
.PHONY: clean
clean:
	rm -f $(VGPS_DIR)/*.o $(OUT_GPS_CREATOR)
	rm -f $(SRC_DIR)/*.o $(OUT_GPS_DATACAR)

# If necessary, setting the library path
.PHONY: set_ld_library_path
set_ld_library_path:
	export LD_LIBRARY_PATH=$(VGPS_DIR):$(EXT_DATA_DIR)

