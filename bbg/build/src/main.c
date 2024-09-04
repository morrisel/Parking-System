/**
 * main.c
 *
 * This module is the main entry point of the application. It is responsible for 
 * initializing GPIO and UART, reading data from UART, processing received messages,
 * and sending acknowledgment responses.
 *
 * The application reads data from UART1 and expects a fixed-length message. 
 * Upon receiving a complete message, it processes the message and sends an 
 * acknowledgment back via UART.
 *
 * Version: v1.0
 * Date:    04-09-2024
 * Author:  Morris
 *
 * Date:            Name:            Version:    Modification:
 * 04-09-2024       Morris           v1.0        created
 *
 */

#include "gpio/gpio.h"
#include "uart/uart.h"
#include "data_formatter.h"
#include <stdio.h>
#include <unistd.h>

#define MESSAGE_LENGTH 13    /* Define the length of the message to be received */


/**
 * main
 *
 * The main function of the application. It initializes GPIO and UART, then enters 
 * a loop where it continuously reads data from UART, processes complete messages,
 * and sends acknowledgment responses.
 *
 * The function sets up GPIO pins for UART communication, configures UART with 
 * the desired parameters, and then enters an infinite loop to handle incoming data.
 * Received data is processed to extract complete messages, and responses are sent 
 * back to the sender.
 *
 * Return: 0 on successful execution.
 */
int main()
{
    /*	FILE *output_file = fopen("ext_data.txt", "a"); */
    /*	if (output_file == NULL) */
    /*	{ */
    /*		perror("Error opening ext_data.txt"); */
    /*		return -1; */
    /*	} */

    /* Initialize GPIO for UART communication */
    gpio_t gpio_tx, gpio_rx;

    /* Set up GPIO for UART transmission (TX pin) */
    gpio_init(&gpio_tx, 15);   /* GPIO_15 (pin 24) */

    /* Set up GPIO for UART reception (RX pin) */
    gpio_init(&gpio_rx, 14);   /* GPIO_14 (pin 26) */

    /* Configure GPIO directions */
    gpio_set_direction(&gpio_tx, "out");  /* TX pin as output (Set the direction of GPIO_15) */
    gpio_set_direction(&gpio_rx, "in");    /* RX pin as input (Set the direction of GPIO_14) */

    /* Setting up UART1 with specified parameters */
    uart_t uart1;
    uart_init(&uart1, "/dev/ttyS1", BAUD_9600, 'N', 8, 1, 1, 1);

    /* Check if UART1 was successfully opened */
    if (!uart_is_open(&uart1))
    {
        fprintf(stderr, "Failed to open UART1.\n");
        return -1;
    }

    /* Buffer for storing received messages */
    char received_message[MESSAGE_LENGTH + 1];
    ssize_t bytes_read;         /* Number of bytes read from UART */
    const char *response_message = "ACK"; /* Response message to be sent */
    size_t message_length = 0;  /* Length of the currently received message */
    int message_started = 0;   /* Flag to indicate if a message has started */


    /**
     * Main loop to continuously read data from UART and process it.
     *
     * The loop reads one byte of data at a time, checks if the message has started,
     * and accumulates bytes until a complete message is received. Once a complete
     * message is received, it is processed, and an acknowledgment response is sent.
     */
    while (1)
    {
        /* Read data from UART1_RXD */
        bytes_read = uart_read_data(&uart1, &received_message[message_length], 1);
        if (bytes_read > 0)
        {
            char received_char = received_message[message_length];

            /* Check if the message has started */
            if (!message_started)
            {
                /* Start a new message if 'D' or 'S' is received */
                if (received_char == 'D' || received_char == 'S')
                {
                    message_started = 1;
                    received_message[0] = received_char;
                    message_length = 1;
                }
            }
            else
            {
                /* Continue receiving the message */
                received_message[message_length++] = received_char;

                /* Check if the message is complete */
                if (message_length == MESSAGE_LENGTH)
                {
                    received_message[message_length] = '\0';  /* Null-terminate the string */
                    printf("Received complete message: %s\n", received_message);

                    /* Process the complete message */
                    process_received_message(received_message);

                    /* Reset message state for the next message */
                    message_started = 0;
                    message_length = 0;

                    /* Send acknowledgment response */
                    ssize_t bytes_written = uart_write_data(&uart1, response_message);
                    if (bytes_written > 0)
                    {
                        /* Confirm that the message was sent successfully */
                        printf("Sending answer message: %s\n", response_message);
                    }
                    else
                    {
                        /* Handle error in writing to UART */
                        fprintf(stderr, "Can't write data to UART\n");
                    }
                }
            }
        }
        else
        {
            /* Handle error in reading from UART */
            fprintf(stderr, "Can't read data from UART\n");
        }

        /* Sleep for 100 ms before the next iteration */
        usleep(100000);
    }

    /* Clean up resources before exiting */
    gpio_deinit(&gpio_tx);
    gpio_deinit(&gpio_rx);
    uart_deinit(&uart1);

    return 0;
}

