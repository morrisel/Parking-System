#ifndef UART_H
#define UART_H

#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <termios.h>

typedef enum
{
    BAUD_2400   = B2400,
    BAUD_9600   = B9600,
    BAUD_19200  = B19200,
    BAUD_38400  = B38400,
    BAUD_57600  = B57600,
    BAUD_115200 = B115200
} BaudRate;

typedef struct
{
    BaudRate       baudRate;                                         /* Baud rate */
    char           parity;                                           /* Parity ('N' - none, 'E' - even, 'O' - odd) */
    uint32_t       databits;                                         /* Number of data bits (7 or 8) */
    uint32_t       stopbits;                                         /* Number of stop bits (1 or 2) */
    uint32_t       vmin;                                             /* Minimum number of bytes to read */
    uint32_t       timeout;                                          /* Read timeout (in deciseconds) */
    int            fd;                                               /* File descriptor for UART */
    bool           isOpen;                                           /* Is the device open? */
    struct termios uart;                                             /* UART configuration */
} uart_t;

void uart_init(uart_t      *uart,
               const char  *path,
               speed_t     baudRate,
               char        parity, 
               uint32_t    databits,
               uint32_t    stopbits,
               uint32_t    vmin, 
               uint32_t    timeout);

void uart_deinit(uart_t *uart);

bool uart_is_open(const uart_t *uart);

ssize_t uart_write_data(uart_t *uart, const char *buff);
ssize_t uart_read_data(uart_t *uart, char *buff, uint32_t sizeRead);

ssize_t uart_send_struct(uart_t *uart, const void *data, size_t size);
ssize_t uart_receive_struct(uart_t *uart, void *data, size_t size);

bool uart_init_device(uart_t *uart);

#endif /* UART_H */

