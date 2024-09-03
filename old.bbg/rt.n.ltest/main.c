#include "gpio.h"
#include "uart.h"
#include <stdio.h>
#include <unistd.h>

/**
 * main - Entry point of the program.
 *
 * This function initializes the GPIOs and UART interface, then enters
 * a loop where it reads data from UART, processes it, and sends a response.
 * The loop runs indefinitely, and the program will continue to operate
 * until it is terminated by an external signal.
 *
 * Return: 0 on successful execution, -1 on failure.
 */
int main() 
{
    // Setting up GPIO to control UART1_TXD and UART1_RXD
    gpio_t gpio_tx, gpio_rx;

    // Initialize GPIO for UART transmission (GPIO_15, pin 24)
    gpio_init(&gpio_tx, 15);   // GPIO_15 (pin 24)

    // Initialize GPIO for UART reception (GPIO_14, pin 26)
    gpio_init(&gpio_rx, 14);   // GPIO_14 (pin 26)

    // Set the direction of GPIO_15 as output (for TX)
    gpio_set_direction(&gpio_tx, "out");

    // Set the direction of GPIO_14 as input (for RX)
    gpio_set_direction(&gpio_rx, "in");

    // Setting up UART1 with specified parameters
    uart_t uart1;
    uart_init(&uart1, "/dev/ttyS1", BAUD_9600, 'N', 8, 1, 1, 1);

    // Verify that UART1 was opened successfully
    if (!uart_is_open(&uart1))
    {
        fprintf(stderr, "Failed to open UART1.\n");
        return -1;
    }


    char received_message[256];
    ssize_t bytes_read;
    const char *response_message = "ACK"; // message for sending the answer

    while (1)
    {
        // Read data from UART1_RXD
        bytes_read = uart_read_data(&uart1, received_message, sizeof(received_message) - 1);
        if (bytes_read > 0) 
        {
            // Null-terminate the received message
            received_message[bytes_read] = '\0';

            // Print the received message to stdout
            printf("Received message: %s\n", received_message);

            // Sending message answer from UART1_TXD
            ssize_t bytes_written = uart_write_data(&uart1, response_message);
            if (bytes_written > 0) 
            {
                // Confirm that the message was sent successfully
                printf("Sending answer message: %s\n", response_message);
            }
            else 
            {
                // Handle error in writing to UART
                fprintf(stderr, "Can't write data to UART\n");
            }
        }
        else
        {
            // Handle error in reading from UART
            fprintf(stderr, "Can't read data from UART\n");
        }

        // Sleep for 100 ms before the next iteration
        usleep(100000);

    }


    // Deinitialize UART and GPIO
    gpio_deinit(&gpio_tx);
    gpio_deinit(&gpio_rx);
    uart_deinit(&uart1);

    return 0;

}
