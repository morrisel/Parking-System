#include "uart.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

/*
 * uart_init - Initializes the UART interface
 * @uart: The UART structure to initialize
 * @path: The file path of the UART device (e.g., "/dev/ttyS0")
 * @baudRate: The baud rate for communication (e.g., B9600)
 * @parity: Parity setting ('N' for none, 'E' for even, 'O' for odd)
 * @databits: Number of data bits (typically 5, 6, 7, or 8)
 * @stopbits: Number of stop bits (1 or 2)
 * @vmin: Minimum number of characters to read
 * @timeout: Timeout value for read operations in deciseconds
 *
 * This function opens the specified UART device and configures it
 * with the provided settings. If the device fails to open or
 * initialize, an error is reported and the function exits.
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
 * uart_deinit - Deinitializes the UART interface
 * @uart: The UART structure to deinitialize
 *
 * This function closes the UART device if it is open, marking it as closed.
 */
void uart_deinit(uart_t *uart) 
{
    if (uart->isOpen && close(uart->fd) == 0) 
    {
        uart->isOpen = false;
    }
}

/*
 * uart_is_open - Checks if the UART interface is open
 * @uart: The UART structure to check
 *
 * Returns true if the UART device is open, false otherwise.
 */
bool uart_is_open(const uart_t *uart) 
{
    return uart->isOpen;
}

/*
 * uart_write_data - Writes data to the UART interface
 * @uart: The UART structure to write to
 * @buff: The data buffer to write
 *
 * This function writes the specified data buffer to the UART device.
 * Returns the number of bytes written, or -1 if an error occurred.
 */
ssize_t uart_write_data(uart_t *uart, const char *buff) 
{
    return write(uart->fd, buff, strlen(buff));
}

/*
 * uart_read_data - Reads data from the UART interface
 * @uart: The UART structure to read from
 * @buff: The buffer to store the read data
 * @sizeRead: The maximum number of bytes to read
 *
 * This function reads data from the UART device into the provided buffer.
 * Returns the number of bytes read, or -1 if an error occurred.
 */
ssize_t uart_read_data(uart_t *uart, char *buff, uint32_t sizeRead) 
{
    return read(uart->fd, buff, sizeRead);
}

/*
 * uart_init_device - Configures the UART device settings
 * @uart: The UART structure representing the device
 *
 * This function configures the UART device according to the settings
 * provided during initialization. It sets the baud rate, parity, data bits,
 * stop bits, and other control flags. If the configuration fails,
 * the function returns false.
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

    uart->uart.c_cflag &= ~PARENB;     // Clear parity enable
    uart->uart.c_cflag &= ~CSTOP;      // Clear stop bits
    uart->uart.c_cflag &= ~CSIZE;      // Clear character size bits

    uart->uart.c_iflag = IGNBRK;       // Ignore break condition on input
    uart->uart.c_lflag = 0;            // No local modes
    uart->uart.c_oflag = 0;            // No output modes

    uart->uart.c_cflag |= ~PARENB;     // Set parity bits
    uart->uart.c_cflag |= ~CSTOP;      // Set stop bits
    uart->uart.c_cflag |= ~CSIZE;      // Set data bits

    uart->uart.c_iflag &= ~(IXON | IXOFF | IXANY);  // Disable software flow control

    uart->uart.c_cc[VTIME] = uart->timeout;     // Set timeout for read operations
    uart->uart.c_cc[VMIN] = uart->vmin;         // Set minimum characters to read

    uart->uart.c_cflag |= (CLOCAL | CREAD);     // Enable receiver and set local mode

    if (tcsetattr(uart->fd, TCSANOW, &uart->uart)) 
    {
        return false;
    }

    return true;
}

