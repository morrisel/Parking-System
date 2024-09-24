#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>  /* Include this for ssize_t */
#include <sys/types.h>  /* Include this for ssize_t */

typedef enum
{
    BAUD_2400 = B2400,
    BAUD_9600 = B9600,
    BAUD_19200 = B19200,
    BAUD_38400 = B38400,
    BAUD_57600 = B57600,
    BAUD_115200 = B115200
} BaudRate;

typedef struct
{
    BaudRate baudRate;
    char parity;
    uint32_t databits;
    uint32_t stopbits;
    uint32_t vmin;
    uint32_t timeout;
    int fd;
    bool isOpen;
    struct termios uart;
} uart_t;

void uart_init(uart_t *uart, const char *path, BaudRate baudRate, char parity, uint32_t databits, uint32_t stopbits, uint32_t vmin, uint32_t timeout);
void uart_deinit(uart_t *uart);

bool uart_is_open(const uart_t *uart);
ssize_t uart_write_data(uart_t *uart, const char *buff);
ssize_t uart_read_data(uart_t *uart, char *buff, uint32_t sizeRead);

/* Functions to send and receive structs */
ssize_t uart_send_struct(uart_t *uart, const void *data, size_t size);
ssize_t uart_receive_struct(uart_t *uart, void *data, size_t size);

bool uart_init_device(uart_t *uart);

#endif

