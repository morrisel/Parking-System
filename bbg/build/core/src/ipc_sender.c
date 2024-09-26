/**
 * ipc_sender.c: Sends data from a function to a FIFO (named pipe)
 *
 * This program reads GPS data from a function and writes it to a
 * named FIFO (First In, First Out) file. It uses signal handling to
 * gracefully handle interruptions (e.g., Ctrl+C).
 *
 * Compilation:
 *      gcc ipc_sender.c -o out_ipc_sender -lrt
 *
 * Usage:
 *      ./out_ipc_sender
 *
 * Features:
 * - Retrieves data from a function 'get_formatted_data()'.
 * - Writes data to a FIFO defined by FIFO_PATH.
 * - Handles SIGINT (Ctrl+C) to allow graceful shutdown.
 *
 * Version: v2.0
 * Date:    01-09-2024
 * Author:  Morris
 *
 * Date:            Name:               Version:        Modification:
 *   10-05-2024       Morris              v1.0            created
 *   31-08-2024       Morris              v1.0            for signal_handler function parameter is unused
 *                                                        optimized flags for compilation 
 *   01-09-2024       Morris              v2.0            added inotify mechanism for monitoring file changes
 *
 */

#include "data_struct_format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define SHM_KEY 1234

#define FIFO_PATH "tmp/gps_pipe"                                     /* Path to the named pipe (FIFO) */
#define BUFFER_SIZE 256                                              /* Buffer size for reading/writing data */

volatile sig_atomic_t running = 1;                                   /* Flag to control program execution */

/**
 * signal_handler - Handles SIGINT to allow graceful shutdown.
 *
 * @signum: The signal number received.
 *
 * This function sets the running flag to 0, indicating that the main
 * loop should terminate.
 */
void signal_handler(int signum __attribute__((unused)))
{   
    /* For using for future - logging implementation */
    running = 0;                                                     /* Set flag to stop the main loop */
}

const char *get_formatted_data(); /* Declaration of the function you will use */

int main()
{
    int      fifo_fd;                                                /* Descriptor for FIFO file */
    char     buffer[BUFFER_SIZE];                                    /* Buffer to hold data */
    ssize_t  bwr;                                                    /* Number of bytes written */

    /* Check if FIFO exists, create if not */
    if (access(FIFO_PATH, F_OK) == -1)
    {
        /* FIFO doesn't exist, create it */
        if (mkfifo(FIFO_PATH, 0666) == -1)
        {
            perror("Error creating FIFO");
            exit(EXIT_FAILURE);
        }
    }

    /* Open the FIFO for writing (non-blocking and synchronous) */
    fifo_fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK | O_SYNC);
    if (fifo_fd == -1)
    {
        perror("Error opening FIFO");
        exit(EXIT_FAILURE);
    }

    /* Set up signal handler for SIGINT (Ctrl+C) */
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        perror("Error setting signal handler");
        close(fifo_fd);                                              /* Close FIFO if signal setup fails */
        exit(EXIT_FAILURE);
    }


    /* Monitor the file for changes and write to FIFO when it's modified */
    while (running)
    {
        const char *data = get_formatted_data();
        if (data != NULL)
        {
            strncpy(buffer, data, BUFFER_SIZE); /* Copy data to buffer */
            buffer[BUFFER_SIZE - 1] = '\0';     /* Ensure null-terminated */
            bwr = write(fifo_fd, buffer, strlen(buffer));
            if (bwr == -1)
            {
                if (errno == EAGAIN)
                {
                    /* FIFO full, try again later */
                    usleep(10000);                           /* Wait for 10 milliseconds */
                    continue;
                }
                else
                {
                    perror("Error writing to FIFO");
                    close(fifo_fd);
                    exit(EXIT_FAILURE);
                }
            }
        }
        else
        {
            usleep(10000); /* Wait for 10 milliseconds if no data is available */
        }

    }

    /* Cleanup */
    close(fifo_fd);

    return 0;
}

