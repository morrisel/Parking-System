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
 * 26-09-2024       Morris           v1.1        add mechanism for receiving structs
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
void uart_init(uart_t *uart, const char *path, speed_t baudRate, char parity, uint32_t databits, uint32_t stopbits, uint32_t vmin, uint32_t timeout)
{
    uart->baudRate = baudRate;
    uart->parity = parity;
    uart->databits = databits;
    uart->stopbits = stopbits;
    uart->vmin = vmin;
    uart->timeout = timeout;
    uart->fd = -1;
    uart->isOpen = false;

    /* Open UART device */
    uart->fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);             /* Use O_NDELAY for non-blocking access */
    if (uart->fd < 0)
    {
        perror("Failed to open port");
        return;
    }

    printf("UART opened successfully on %s\n", path);

    if (!uart_init_device(uart))
    {
        uart_deinit(uart);
        perror("Failed to initialize UART device");
    }
    else
    {
        printf("UART initialized successfully\n");                   /* Debug message */
    }

    uart->isOpen = true;
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


ssize_t uart_send_struct(uart_t *uart, const void *data, size_t size)
{
    return write(uart->fd, data, size);
}


ssize_t uart_receive_struct(uart_t *uart, void *data, size_t size)
{
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
    if (uart->fd < 0)
    {
        return false;
    }

    tcgetattr(uart->fd, &uart->uart);
    cfsetispeed(&uart->uart, uart->baudRate);
    cfsetospeed(&uart->uart, uart->baudRate);

    uart->uart.c_cflag &= ~PARENB;                                   /* Disable parity */
    uart->uart.c_cflag &= ~CSTOPB;                                   /* 1 stop bit */
    uart->uart.c_cflag &= ~CSIZE;                                    /* Mask the character size bits */
    uart->uart.c_cflag |= CS8;                                       /* 8 data bits */

    uart->uart.c_cflag |= (CLOCAL | CREAD);
    uart->uart.c_iflag &= ~(IXON | IXOFF | IXANY);                   /* No software flow control */
    uart->uart.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);           /* Raw input */

    tcsetattr(uart->fd, TCSANOW, &uart->uart);

    return true;
}
