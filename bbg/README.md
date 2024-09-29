# BeagleBone Green (BBG) Technical Documentation for Parking System

## Introduction
This document provides detailed information about the components running on the BeagleBone Green (BBG) as part of the Parking System project. The BBG acts as an intermediary device that receives data from the GPS sensor located on another device and forwards it to the central server. The software components on the BBG handle data formatting, inter-process communication (IPC), and data transmission to the server.

## Components and Functionality

### 1. **data_formatter.c**
   - **Description:** This module is responsible for both formatting data packets and managing shared memory for inter-process communication (IPC). It allows structured data (`DataPacket`) to be formatted into a human-readable format and also facilitates the storage of this data in shared memory for other processes to access.
   - **Functionality:**
     - Formats `DataPacket` structures into human-readable strings.
     - Manages shared memory segments for storing `DataPacket` data.
     - Provides functions to attach, detach, and remove shared memory.
     - Ensures that the formatted data is consistent and ready for inter-process communication.

### 2. **ipc_sender.c**
   - **Description:** This program reads GPS data from a shared memory segment and writes it to a FIFO (named pipe). It handles signal interruptions `SIGINT` to ensure data consistency and uses formatted data from the `data_struct_format.c` module before transmission.
   - **Functionality:**
     - Reads GPS data from shared memory.
     - Writes data to a FIFO defined by `FIFO_PATH`.
     - Gracefully handles signals like `SIGINT` to ensure a safe shutdown.
     - Uses `get_formatted_data` from `data_struct_format.c` to format the data before sending.

     - **Key Features**:
       - Inotify mechanism replaced with shared memory for monitoring file changes.
       - Signal handling for interruption (e.g., Ctrl+C) to allow safe termination.
       - Modular integration with `data_struct_format` for data formatting.

### 3. **sys_com_controller.c**
   - **Description:** The `sys_com_controller.c` module initializes UART communication and sets up GPIO pins for TX and RX. It listens for incoming data over UART, processes it as `DataPacket` structures, and then uses the `data_struct_format.c` module to format and display the data. This program ensures reliable communication with the STM32 Nucleo-F746ZG board and prepares data for further processing.
   - **Functionality:**
     - Initializes UART and GPIO pins for communication.
     - Reads incoming data from UART.
     - Processes and formats the received data using `data_struct_format.c`.
     - Displays data in both hexadecimal and human-readable formats.
     - Sends acknowledgment responses back via UART if necessary.

### 4. **tcp_client.c**
   - **Description:** This client program reads data from a FIFO and sends it to a TCP server. It is the final stage in the data transmission pipeline on the BBG.
   - **Functionality:**
     - Reads data from a FIFO (`tmp/gps_pipe`).
     - Connects to a TCP server using specified IP and port.
     - Sends the data, prefixed with the client’s MAC address, to the server.

## Inter-Process Communication (IPC)
The BBG utilizes various IPC mechanisms to facilitate communication between different processes:
- **FIFO (Named Pipes):**
  - `tmp/gps_pipe`: Used to transfer data from `ipc_sender` to `tcp_client`.

The FIFOs ensure that data is sequentially processed and forwarded to the appropriate modules or the central server.

## Compilation and Build

### Makefile
The Makefile provided in the project directory is used to compile all the source files. Below are the commands to use:

#### Compilation
To compile the programs, navigate to the project directory and run:
```sh
make all
```

#### Cleaning Up
To remove all compiled executables, run:
```sh
make clean
```

## Execution and Usage
### Starting the BBG Client Programs

#### 1. Start tcp_client:
Start the `out_tcp_client` program to read from `tmp/gps_pipe` and send the data to the central server via TCP.
```sh
./out_tcp_client
```

#### 2. Start sys_com_controller:
Run the `sys_com_controller` to initiate communication and receive data from the board, and save the processed data to the `ext_data.txt` file.
```sh
./sys_com_controller
```

#### 3. Start ipc_sender:
Run the `out_ipc_sender` program to begin reading GPS data from the `ext_data.txt` file and sending it to `tmp/gps_pipe`.
```sh
./out_ipc_sender
```

