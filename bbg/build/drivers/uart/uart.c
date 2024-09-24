/**
 * uart.c - Functions for handling UART communication.
 *
 * This file contains the implementation of functions to initialize,
 * configure, and interact with UART (Universal Asynchronous
 * Receiver-Transmitter) devices. The code is designed to provide
 * a simple interface for opening, closing, reading from, and writing
 * to a UART port. It handles the configuration of UART parameters
 * such as baud rate, parity, data bits, and stop bits.
 *
 * Note: The code assumes the presence of a valid UART configuration
 * structure (uart_t) and does not perform any high-level error
 * handling beyond reporting failures.
 *
 * Version: v1.0
 * Date:    04-09-2024
 * Author:  Morris
 *
 * Date:            Name:            Version:    Modification:
 * 04-09-2024       Morris           v1.0        created
 * 24-09-2024       Morris           v1.1        add mechanism for receiving structs
 *
 */

#include "uart.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>



/**
 * uart_init - Initialize the UART port with the specified settings.
 * @uart: Pointer to the uart_t structure to initialize.
 * @path: Path to the UART1 device (e.g., /dev/ttyS1).
 * @baudRate: Baud rate for communication.
 * @parity: Parity setting ('N' for None, 'E' for Even, 'O' for Odd).
 * @databits: Number of data bits (usually 7 or 8).
 * @stopbits: Number of stop bits (1 or 2).
 * @vmin: Minimum number of characters to read.
 * @timeout: Timeout value for read operations.
 *
 * This function attempts to open the UART device specified by
 * @path, configure it with the provided settings, and prepare
 * it for communication. If the initialization fails, the UART
 * port is closed, and an error is reported.
 */
void uart_init(uart_t *uart, const char *path, BaudRate baudRate, char parity, uint32_t databits, uint32_t stopbits, uint32_t vmin, uint32_t timeout)
{
    /* Save configuration parameters in the uart_t structure */
    uart->baudRate = baudRate;
    uart->parity = parity;
    uart->databits = databits;
    uart->stopbits = stopbits;
    uart->vmin = vmin;
    uart->timeout = timeout;
    uart->fd = -1;
    uart->isOpen = false;

    /* Specify the path to the UART device */
    uart->fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);
    if (uart->fd < 0) {
/*        perror("open uart device"); */
        perror("Failed to open port");
        return;
    }


/*    if (!uart_init_device(uart)) */
/*    { */
/*        uart_deinit(uart); */
/*        perror("Failed to initialize uart"); */
/*    } */

/*    uart->isOpen = true; */

}

/*
 * uart_deinit - Close the UART port.
 * @uart: Pointer to the uart_t structure representing the UART device.
 *
 * This function closes the UART port if it is open, and marks it as closed.
 */
void uart_deinit(uart_t *uart)
{
    if (uart->isOpen) {
        close(uart->fd);
        uart->isOpen = false;
    }
}

/*
 * uart_is_open - Check if the UART port is open.
 * @uart: Pointer to the uart_t structure representing the UART device.
 *
 * Returns true if the UART port is open, false otherwise.
 */
bool uart_is_open(const uart_t *uart)
{
    return uart->isOpen;
}

/*
 * uart_write_data - Write data to the UART port.
 * @uart: Pointer to the uart_t structure representing the UART device.
 * @buff: Buffer containing the data to write.
 *
 * Returns the number of bytes written, or -1 if an error occurred.
 */
ssize_t uart_write_data(uart_t *uart, const char *buff)
{
    if (!uart_is_open(uart)) {
        return -1;
    }
    return write(uart->fd, buff, strlen(buff));
}

/*
 * uart_read_data - Read data from the UART port.
 * @uart: Pointer to the uart_t structure representing the UART device.
 * @buff: Buffer to store the read data.
 * @sizeRead: Number of bytes to read.
 *
 * Returns the number of bytes read, or -1 if an error occurred.
 */
ssize_t uart_read_data(uart_t *uart, char *buff, uint32_t sizeRead)
{
    if (!uart_is_open(uart)) {
        return -1;
    }
    return read(uart->fd, buff, sizeRead);
}


ssize_t uart_send_struct(uart_t *uart, const void *data, size_t size)
{
    if (!uart_is_open(uart)) {
        return -1;
    }
    return write(uart->fd, data, size);
}


ssize_t uart_receive_struct(uart_t *uart, void *data, size_t size)
{
    if (!uart_is_open(uart)) {
        return -1;
    }
    return read(uart->fd, data, size);
}

/*
 * uart_init_device - Configure the UART device with the specified settings.
 * @uart: Pointer to the uart_t structure representing the UART device.
 *
 * This function applies the UART settings such as baud rate, parity,
 * data bits, and stop bits to the UART device. It configures the
 * device's termios structure and applies the settings immediately.
 *
 * Returns true if the device was successfully configured, false otherwise.
 */
bool uart_init_device(uart_t *uart)
{
    if (uart->fd < 0) {
        return false;
    }

    /* Get current UART settings */
    if (tcgetattr(uart->fd, &uart->uart) != 0) {
        perror("tcgetattr");
        close(uart->fd);
        return false;
    }

    /* Set baud rate */
    cfsetospeed(&uart->uart, uart->baudRate);
    cfsetispeed(&uart->uart, uart->baudRate);

    /* Configure parity */
    if (uart->parity == 'N') {
        uart->uart.c_cflag &= ~PARENB; /* No parity */
    } else if (uart->parity == 'E') {
        uart->uart.c_cflag |= PARENB;  /* Even parity */
        uart->uart.c_cflag &= ~PARODD;
    } else if (uart->parity == 'O') {
        uart->uart.c_cflag |= PARENB;  /* Odd parity */
        uart->uart.c_cflag |= PARODD;
    }


    /* Set data bits */
    uart->uart.c_cflag &= ~CSIZE;  /* Clear data bits mask */
    switch (uart->databits) {
        case 8:
            uart->uart.c_cflag |= CS8;
            break;
        case 7:
            uart->uart.c_cflag |= CS7;
            break;
        default:
            fprintf(stderr, "Unsupported databits: %u\n", uart->databits);
            close(uart->fd);
            return false;
    }

    /* Set the number of stop bits */
    if (uart->stopbits == 2) {
        uart->uart.c_cflag |= CSTOPB;  /* 2 stop bits */
    } else {
        uart->uart.c_cflag &= ~CSTOPB; /* 1 stop bit */
    }

    /* Set minimum number of bytes for reading */
    uart->uart.c_cc[VMIN] = uart->vmin;
    
    /* Set timeout for data waiting */
    uart->uart.c_cc[VTIME] = uart->timeout;

    /* Apply UART settings */
    if (tcsetattr(uart->fd, TCSANOW, &uart->uart) != 0) {
        perror("tcsetattr");
        close(uart->fd);
        return false;
    }

    uart->isOpen = true;
    return true;
}
