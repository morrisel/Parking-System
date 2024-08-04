/**
 * ipc_sender.c: Sends data from a file to a FIFO (named pipe)
 *
 * This program reads GPS data from a specified file and writes it to a
 * named FIFO (First In, First Out) file. It uses signal handling to
 * gracefully handle interruptions (e.g., Ctrl+C).
 *
 * Compilation:
 *      gcc ipc_sender.c -o out_ipc_sender
 *
 * Usage:
 *      ./out_ipc_sender
 *
 * Features:
 * - Reads from a GPS data file located at 'tmp/gps_fifo'.
 * - Writes data to a FIFO defined by FIFO_PATH.
 * - Handles SIGINT (Ctrl+C) to allow graceful shutdown.
 *
 * Version: v1.0
 * Date:    10-05-2024
 * Author:  Morris
 *
 * Date:            Name:               Version:        Modification:
 *   10-05-2024       Morris              v1.0            created
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

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
/* Signal handler for graceful shutdown */
void signal_handler(int signum)
{
    running = 0;	                                                 /* Set flag to stop the main loop */
}

int main()
{
    int      fifo_fd;                                                /* Descriptor for FIFO file */
    char     buffer[BUFFER_SIZE];                                    /* Buffer to hold data */
    ssize_t  bwr;                                                    /* Number of bytes written */
    FILE     *dfile;                                                 /* File pointer for the data file */


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

    /* Read data from a 'gps_fifo' file and send it to the FIFO */
    dfile = fopen("tmp/gps_fifo", "r");
    if (dfile == NULL)
    {
        perror("Error opening data file");
        close(fifo_fd);                                              /* Close FIFO if file opening fails */
        exit(EXIT_FAILURE);
    }

    /* Read GPS data from file and write to FIFO */
    while (fgets(buffer, BUFFER_SIZE, dfile) != NULL && running)
    {
        sleep(1);
        bwr = write(fifo_fd, buffer, strlen(buffer));
        if (bwr == -1)
        {
            if (errno == EAGAIN)
            {
                /* FIFO full, try again later */
                usleep(10000);                                       /* Wait for 10 milliseconds */
                continue;
            }
            else
            {
                perror("Error writing to FIFO");
                exit(EXIT_FAILURE);
            }
        }
    }

    /* Cleanup */
    fclose(dfile);                                                   /* Close the data file */
    close(fifo_fd);                                                  /* Close the FIFO */

    return 0;
}
