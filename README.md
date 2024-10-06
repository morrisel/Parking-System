# Parking System Technical Documentation

## Introduction

The Parking System is designed to monitor and manage parking spaces in real-time. It utilizes a client-server architecture where parking sensors (clients) communicate with a central server for data processing and storage. The system leverages multithreading, parallel processing, and inter-process communication (IPC) mechanisms to achieve efficient and reliable operation in a Linux environment.


## Components and Functionality
### Server:
The core server program runs on a Linux machine, listening for TCP connections from clients (such as the BeagleBone Green). It receives, processes, and stores data for further analysis and decision-making.

### BeagleBone Green: 
The BeagleBone Green acts as an intermediary module, receiving GPS data from the STM32 NUCLEO-F746ZG and forwarding it to the Linux server. It manages communication between the STM32 board and the central server.

### STM32 NUCLEO-F746ZG:
The STM32 board collects location data and transmits it to the BeagleBone Green for further relay to the server.


##### Conclusion:
The Parking System is a robust and scalable solution that efficiently manages parking spaces in real-time. Its modular design, based on standard technologies such as multithreading, parallel processing, and IPC mechanisms, allows for flexible adaptation and scalability. By focusing on real-time data processing and storage, the system ensures that parking information is always up-to-date and available for decision-making and analysis.


Base Directory Structure:
Parking-System/
├── bbg/
│   ├── bin/
│   ├── build/
│   ├── docs/
│   ├── License
│   └── README.md
├── nuctobbg/
│   ├── Core/
│   ├── Debug/
│   ├── Drivers/
│   ├── nuctobbg Debug.launch
│   ├── nuctobbg.ioc
│   ├── STM32F746ZGTX_FLASH.ld
│   └── STM32F746ZGTX_RAM.ld
├── README.md
└── Server/
    ├── bin/
    ├── build/
    ├── docs/
    └── README.md


Each sub-project (Server, bbg, nuctobbg) contains its own documentation and codebase, with README.md files in their respective directories providing more detailed technical descriptions specific to that module.
