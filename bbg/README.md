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
   - **Description:** This program reads GPS data from a file and sends it to a FIFO (named pipe). It facilitates the transfer of sensor data to other processes on the BBG or the central server.

   - **Description:**  

   - **Functionality:**
   
     - Reads data from `ext_data.txt`.
     - Sends data to a FIFO defined by `FIFO_PATH`.
     - Handles interruptions gracefully, ensuring data consistency.

### 3. **sys_com_controller.c**
   - **Description:** The `sys_com_controller` module is the entry point for the BBG client application. It initializes communication with the STM32 Nucleo-F746ZG board via the GPIO interface. This program will receive data from the board, process it using the `data_formatter.c` module, and save the processed data to the `ext_data.txt` file.
   - **Functionality:**
     - Reads data from UART1.
     - Processes and validates the received messages.
     - Sends acknowledgment responses back via UART.

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

