# Parking System Server Technical Documentation


## Introduction
This document provides detailed information about the Server of the Parking System project. This is a multi-client server system that receives data from clients, including their IP addresses, MAC addresses, and GPS data. The server records and updates client information in the database.


## Components and Functionality
### Server Side (Linux)
1.  **out_server:**
    - Core server program that listens for TCP connections from clients (parking sensors).
    - Receives data from clients, writes it to shared memory, and handles concurrent client connections.
    - Uses a semaphore to limit the number of simultaneous clients.
2.  **out_listener:**
    - Monitors changes in shared memory.
    - When data changes, it sends a notification through a FIFO (named pipe) to `out_giis`.
3.  **out_giis:**
    - Reads data from shared memory when notified by `out_listener`.
    - Appends the data to a file (`giis/gdfs.data`) and a FIFO (`giis/ipc_to_db`).
4.  **out_insert_data_from_giis_shm:**
    - Reads data from both `giis/gdfs.data` and the FIFO `giis/ipc_to_db`.
    - Parses the data (MAC address, status, coordinates) and inserts it into an SQLite database (`prksys_db.db`).
5.  **out_update_prices:**
    - Updates parking prices in the database based on a price file (`prices.txt`).
    - Adds new prices, modifies existing ones, and removes prices that are not present in the file.


### Data Flow
1.  **Data Acquisition:**
    - The GPS sensor data is sent to `tmp/gps_pipe`.
    - `out_ipc_sender` reads this data and writes it to `tmp/gps_pipe`.
    - `out_tcp_client` reads from `tmp/gps_pipe`, adds the client's MAC address, and sends the combined data to `out_server`.
2.  **Data Processing and Storage:**
    - `out_server` receives the data, writes it to shared memory, and prints it to the console.
    - `out_listener` detects the change in shared memory and notifies `out_giis`.
    - `out_giis` reads the data from shared memory, writes it to `giis/gdfs.data` and the FIFO `giis/ipc_to_db`.
    - `out_insert_data_from_giis_shm` reads the data from both the file and FIFO and inserts it into the SQLite database.

##### Compilation and Execution
###### Compilation
The project is compiled using the provided Makefile. Running `make all` from the project directory will compile all source files and generate the corresponding executables.
```sh
make all
```
To clean up compiled executables, run:
```sh
make clean
```
###### Execution
There is a dedicated script to run the system: `out_prk_sys_srv_run`. This program is responsible for executing the server-side programs as background processes and monitoring them. To start the system, execute:
```sh
./out_prk_sys_srv_run
```
#
##### Configuration
*  **Configuration Files:**
   * Ensure that the FIFO files (`gps_pipe and ipc_to_db`) are created before running the programs.
   * Modify `prices.txt` to update parking prices.
*  **Adjustable Parameters:**
   * The number of simultaneous client connections can be adjusted by modifying the semaphore initialization in out_server.

##### Usage
*  **Starting the System:**
   * Execute `./out_prk_sys_srv_run` to start the server-side programs.
*  **Stopping the System:**
   * Terminate the background processes using the appropriate kill commands.
*  **Monitoring and Troubleshooting:**
   * Monitor the console output for logs and errors.
   * Check the contents of `giis/gdfs.data` and the database (`prksys_db.db`) for stored data.

##### Multithreading and Parallelism
The Parking System utilizes multithreading and parallel processing extensively:
*  **Server-Side Multithreading:** `out_server` employs multithreading to handle multiple client connections concurrently. Each client is managed by a separate thread, allowing the server to process data from multiple sensors simultaneously.
*  **Parallel Data Processing:** `out_giis` and `out_insert_data_from_giis_shm` work in parallel to process incoming parking data. This ensures efficient data processing and minimal latency.
* **Client-Side Concurrency:** `out_ipc_sender` and `out_tcp_client` operate concurrently to ensure a smooth flow of data from sensors to the server.

##### Inter-Process Communication (IPC)
The Parking System employs various IPC mechanisms:
*  **Shared Memory:** Used for communication between `out_server, out_listener`, and `out_giis`. The server writes incoming data to shared memory, which is monitored by `out_listener`.
*  **FIFOs (Named Pipes):**
   * `tmp/gps_pipe`: Transfers data from `out_ipc_sender` to `out_tcp_client`.
   * `giis/ipc_to_db`: Transfers data from `out_giis` to `out_insert_data_from_giis_shm`.
*  **TCP Sockets:** Used for reliable, bidirectional communication between the parking sensors (clients) and the central server (`out_server`).

##### Conclusion
The Parking System is a well-designed solution that leverages multithreading, parallel processing, and IPC mechanisms to efficiently manage parking spaces. Its modular architecture and use of standard technologies make it adaptable and scalable. The system's focus on real-time data processing and storage ensures that parking information is readily available for analysis and decision-making.



