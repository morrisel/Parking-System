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
 * @path: Path to the UART device (e.g., /dev/ttyS0).
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
    uart->baudRate = baudRate;
    uart->parity = parity;
    uart->databits = databits;
    uart->stopbits = stopbits;
    uart->vmin = vmin;
    uart->timeout = timeout;
    uart->fd = -1;
    uart->isOpen = false;

    uart->fd = open(path, O_RDWR | O_NOCTTY);
    if (uart->fd == -1) 
    {
        perror("Failed to open port");
        return;
    }

    uart->isOpen = true;

    if (!uart_init_device(uart)) 
    {
        uart_deinit(uart);
        perror("Failed to initialize uart");
    }
}

/*
 * uart_deinit - Close the UART port.
 * @uart: Pointer to the uart_t structure representing the UART device.
 *
 * This function closes the UART port if it is open, and marks it as closed.
 */
void uart_deinit(uart_t *uart) 
{
    if (uart->isOpen && close(uart->fd) == 0) 
    {
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
    return read(uart->fd, buff, sizeRead);
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
    if (tcgetattr(uart->fd, &uart->uart)) 
    {
        return false;
    }

    cfsetispeed(&uart->uart, uart->baudRate);
    cfsetospeed(&uart->uart, uart->baudRate);

    uart->uart.c_cflag &= ~PARENB;     /* Clear parity enable */
    uart->uart.c_cflag &= ~CSTOP;      /* Clear stop bits */
    uart->uart.c_cflag &= ~CSIZE;      /* Clear character size bits */

    uart->uart.c_iflag = IGNBRK;       /* Ignore break condition */
    uart->uart.c_lflag = 0;            /* No local modes */
    uart->uart.c_oflag = 0;            /* No output processing */

    uart->uart.c_cflag |= ~PARENB;     /* Set parity bits */
    uart->uart.c_cflag |= ~CSTOP;      /* Set stop bits */
    uart->uart.c_cflag |= ~CSIZE;      /* Set data bits */

    /*    // Set parity, stop bits, and data bits based on user settings */
    /*    uart->uart.c_cflag |= (uart->parity == 'N' ? 0 : (uart->parity == 'E' ? PARENB : (PARENB | PARODD))); */
    /*    uart->uart.c_cflag |= (uart->stopbits == 2 ? CSTOPB : 0); */
    /*    uart->uart.c_cflag |= (uart->databits == 7 ? CS7 : CS8); */

    uart->uart.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable flow control */

    uart->uart.c_cc[VTIME] = uart->timeout;
    uart->uart.c_cc[VMIN] = uart->vmin;

    uart->uart.c_cflag |= (CLOCAL | CREAD); /* Enable receiver and set local mode */

    if (tcsetattr(uart->fd, TCSANOW, &uart->uart)) 
    {
        return false;
    }

    return true;
}

