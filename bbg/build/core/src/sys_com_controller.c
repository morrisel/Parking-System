/**
 * sys_com_controller.c: UART communication controller with GPIO
 *
 * This program initializes UART communication and GPIO pins for TX and RX. It
 * listens for incoming data over UART, processes the received data as a 
 * structured packet, and then formats and displays it. The program runs in 
 * a continuous loop, periodically checking for new data from the UART interface.
 *
 * Compilation:
 *      gcc sys_com_controller.c -o out_sys_com_controller -lgpio -luart
 *
 * Usage:
 *      ./out_sys_com_controller
 *
 * Features:
 * - Initializes GPIO pins for TX (pin 15) and RX (pin 14).
 * - Sets up UART on /dev/ttyS1 with 9600 baud rate.
 * - Reads data from UART and processes it as a DataPacket structure.
 * - Formats and prints the received data in hexadecimal and human-readable format.
 * - Implements basic error handling for UART and GPIO initialization.
 *
 * Version: v1.0
 * Date:    01-10-2024
 * Author:  Morris
 *
 * Date:            Name:               Version:        Modification:
 *   01-10-2024     Morris              v1.0            created
 *
 */

#include "gpio.h"
#include "uart.h"
#include "data_struct_format.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#define MESSAGE_LENGTH sizeof(DataPacket)   /* Length of the expected data packet */

int main()
{
    gpio_t gpio_tx, gpio_rx;                /* GPIO structures for TX and RX pins */

    /* Initialize GPIO pin 15 for TX (transmitting) and set its direction to output. */
    gpio_init(&gpio_tx, 15);
    gpio_set_direction(&gpio_tx, "out");

    /* Initialize GPIO pin 14 for RX (receiving) and set its direction to input. */
    gpio_init(&gpio_rx, 14);
    gpio_set_direction(&gpio_rx, "in");

    uart_t uart1;       /* UART structure for UART communication */

    /* Initialize the UART interface on /dev/ttyS1, with 9600 baud rate, 8 data bits, no parity, 1 stop bit. */
    uart_init(&uart1, "/dev/ttyS1", BAUD_9600, 'N', 8, 1, 1, 1);

    /* Try to open the UART device. If it fails, print an error and return -1 to signal failure. */
    if (!uart_init_device(&uart1)) {
        fprintf(stderr, "Failed to open UART1.\n");
        return -1;
    }

    /* Buffer to accumulate incoming data from UART. It's sized to fit a full DataPacket. */
    char buffer[MESSAGE_LENGTH];
    ssize_t bytes_received;                         /* Variable to track the number of bytes received */

    printf("Struct length: %u\n", MESSAGE_LENGTH);
    puts("=======================");

    /* Main loop to continuously read data from UART and process it. */
    while (1)
    {
        bytes_received = 0;

        /* Continue reading data until the full message (DataPacket) is received. */
        if (bytes_received < MESSAGE_LENGTH)
        {
            ssize_t result = uart_read_data(&uart1, buffer + bytes_received, MESSAGE_LENGTH - bytes_received);
            if (result > 0)
            {
                /* Successfully read data, accumulate the total bytes received. */
                bytes_received += result;
            }
            else if (result == -1 && errno == EAGAIN)
            {
                /* Non-blocking read operation, wait and retry if data is not yet available. */
                usleep(100);  /* Brief pause before retrying */
                continue;
            }
            else if (result == 0)
            {
                /* Connection closed by the sender, exit the loop. */
                printf("Connection closed.\n");
                break;
            }
            else
            {
                /* Handle any other errors during UART read. */
                perror("Error receiving data from UART");
                break;
            }
        }

        /* The full message is received, process the data.  */
        if (bytes_received == MESSAGE_LENGTH)
        {
            printf("Received bytes:\n");
            for (int i = 0; i < MESSAGE_LENGTH; ++i)
            {
                /* Print each byte as a hexadecimal value */
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

            /* Format and display the structured data */
            format_data_struct(received_packet);
            printf("Formatted Data: %s\n", get_formatted_data(received_packet));
        }
        else
        {
            printf("Incomplete packet received.\n");
        }
        puts("");

        /* Brief pause before restarting the loop to avoid CPU overuse. */
        sleep(1);
    }

    /* Clean up the UART and GPIO resources before exiting the program. */
    uart_deinit(&uart1);
    gpio_deinit(&gpio_tx);
    gpio_deinit(&gpio_rx);

    return 0;
}

