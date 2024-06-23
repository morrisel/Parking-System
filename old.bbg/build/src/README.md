# GPS Data Transmission to Server for Parking-System

This project is a component of the Parking-System and is designed to transmit GPS data from a BeagleBone Green (BBG) microcomputer to a remote Linux server.

## Workflow

1.  **GPS Data Collection:** A GPS sensor connected to an STM32 microcontroller transmits location data via the I2C interface.
2.  **Data Transfer to BBG:** The BeagleBone Green (BBG) microcomputer receives the GPS data from the STM32.
3.  **Transmission Process Activation:** Upon receiving GPS data, the BBG triggers the `ipc_connector.c` process (daemon/thread).
4.  **Data Transfer to FIFO:** `ipc_connector.c` reads the GPS data and writes it to a named pipe (FIFO).
5.  **Sending Data to Server:** The `tcp_client.c` program reads data from the FIFO and sends it to the remote Linux server.
6.  **Server-Side Processing:** The Linux server receives the GPS data, processes it (e.g., stores it in a database), and performs other actions required for the Parking-System.

## Components

*   **tcp_client.c:** Client-side program that reads GPS data from the FIFO and sends it to the server.
*   **ipc_connector.c:** Intermediary program that receives GPS data from the STM32 and forwards it to the FIFO. Includes a SIGINT signal handler for graceful shutdown.

## Installation

1.  Compile the programs:

    ```bash
    gcc tcp_client.c -o tcp_client
    gcc ipc_connector.c -o ipc_connector
    ```
2.  Settings

    Create ```gps_pipe``` pipe ```mkfifo tmp/gps_pipe``` manually

    After compilation and setup, your project directory should have the following structure
    
    ```bash
    .
    ├── ipc_connector.c
    ├── out_ipc_connector
    ├── out_tcp_client
    ├── README.md
    ├── tcp_client.c
    └── tmp
        ├── gps_fifo
        └── gps_pipe


3.  Run the programs:

    ```bash
    ./ipc_connector &  # Run in the background
    ./tcp_client
    ```

## Dependencies

*   Linux operating system
*   Named pipe (FIFO)
*   BeagleBone Green (BBG) microcomputer
*   STM32 microcontroller with a connected GPS sensor

## Implementation Notes

*   **Logging:** It is recommended to add logging functionality to track errors and program state.
*   **Configuration:** Consider moving parameters like the FIFO path and buffer size to a configuration file or command-line arguments for greater flexibility.

## License

MIT License

