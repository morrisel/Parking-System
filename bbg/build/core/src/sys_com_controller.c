#include "gpio.h"
#include "uart.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/* The message length we're expecting to receive, defined by the size of the DataPacket structure. */
/* We pack the structure to ensure that no padding bytes are added by the compiler. */
#define MESSAGE_LENGTH sizeof(DataPacket)   /* длина полученного сообщения */

/* This structure represents the data packet we expect to receive via UART. */
/* It contains an operation code (either 'D' or 'S'), followed by three 16-bit integer values (x, y, z). */
#pragma pack(push, 1)
struct __attribute__((packed)) DataPacket
{
    char op_code;      /* Operation code: 'D' for data, 'S' for some other operation (hypothetically). */
    uint16_t x;        /* 16-bit integer, could represent some form of coordinate or measurement. */
    uint16_t y;        /* Another 16-bit integer. */
    uint16_t z;        /* And yet another 16-bit integer. */
};
#pragma pack(pop)
#define DataPacket struct DataPacket    /* Just making things easier to refer to later */

int main()
{
    gpio_t gpio_tx, gpio_rx;

    /* Initialize GPIO pin 15 for TX (transmitting) and set its direction to output. */
    gpio_init(&gpio_tx, 15);
    gpio_set_direction(&gpio_tx, "out");

    /* Initialize GPIO pin 14 for RX (receiving) and set its direction to input. */
    gpio_init(&gpio_rx, 14);
    gpio_set_direction(&gpio_rx, "in");

    uart_t uart1;
    /* Initialize the UART interface on /dev/ttyS1, with 9600 baud rate, 8 data bits, no parity, 1 stop bit. */
    uart_init(&uart1, "/dev/ttyS1", BAUD_9600, 'N', 8, 1, 1, 1);

    /* Try to open the UART device. If it fails, print an error and return -1 to signal failure. */
    if (!uart_init_device(&uart1)) {
        fprintf(stderr, "Failed to open UART1.\n");
        return -1;
    }

    /* Buffer to accumulate incoming data from UART. It's sized to fit a full DataPacket. */
    char buffer[MESSAGE_LENGTH];  /* буфер для накопления данных */
    ssize_t bytes_received;

    printf("Struct length: %u\n", MESSAGE_LENGTH);
    puts("=======================");

    /* Main loop to continuously read data from UART and process it. */
    while (1)
    {
        bytes_received = 0;

        /* We check if we've received the full message. If not, we keep reading. */
        if (bytes_received < MESSAGE_LENGTH)
        {
            ssize_t result = uart_read_data(&uart1, buffer + bytes_received, MESSAGE_LENGTH - bytes_received);
            if (result > 0)
            {
                /* If we successfully read data, we add the number of bytes read to the total. */
                bytes_received += result;
            }
            else if (result == -1 && errno == EAGAIN)
            {
                /* If the read operation would block, we just sleep for a bit and try again. */
                usleep(100);  /* Неблокирующая пауза */
                continue;
            }
            else if (result == 0)
            {
                printf("Connection closed.\n");
                break;
            }
            else
            {
                /* If we encounter an error, print it and break out of the loop. */
                perror("Error receiving data from UART");
                break;
            }
        }

        /* Once we've received the full message, we can start processing it. */
        if (bytes_received == MESSAGE_LENGTH)
        {
            printf("Received bytes:\n");
            for (int i = 0; i < MESSAGE_LENGTH; ++i)
            {
                /* Print each byte as a hexadecimal value, because why not? */
                printf("%02X ", (unsigned char)buffer[i]);
            }
            printf("\n");

            /* Cast the buffer to a DataPacket structure and extract the information. */
            DataPacket *received_packet = (DataPacket *)buffer;

            /* Print out the contents of the received packet in a readable format. */
            printf("Received DataPacket:\n");
            printf("op_code = %c\n", received_packet->op_code);
            printf("x = %u\n", received_packet->x);
            printf("y = %u\n", received_packet->y);
            printf("z = %u\n", received_packet->z);
        }
        else
        {
            /* If we didn't receive a full packet, let the user know. */
            printf("Incomplete packet received.\n");
        }

        puts("");

        /* Sleep for 1 millisecond before repeating the loop to avoid hogging the CPU. */
        usleep(100);
    }

    /* Once we're done, clean up the UART and GPIO resources before exiting. */
    uart_deinit(&uart1);
    gpio_deinit(&gpio_tx);
    gpio_deinit(&gpio_rx);

    return 0;
}

