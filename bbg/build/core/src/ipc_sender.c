/**
 * ipc_sender.c: Sends data from shared memory to a FIFO (named pipe)
 *
 * This program reads GPS data from a shared memory segment and writes it 
 * to a named FIFO (First In, First Out) file. It uses signal handling to
 * gracefully handle interruptions (e.g., Ctrl+C).
 *
 * Compilation:
 *      gcc ipc_sender.c -o out_ipc_sender -lrt
 *
 * Usage:
 *      ./out_ipc_sender
 *
 * Features:
 * - Retrieves data from a shared memory segment.
 * - Writes data to a FIFO defined by FIFO_PATH.
 * - Handles SIGINT (Ctrl+C) to allow graceful shutdown.
 * - Uses the get_formatted_data() function to format data before writing.
 *
 * Version: v3.0
 * Date:    01-09-2024
 * Author:  Morris
 *
 * Date:            Name:               Version:        Modification:
 *   10-05-2024       Morris              v1.0            created
 *   31-08-2024       Morris              v1.0            add unused attribute for signal_handler
 *                                                        optimized flags for compilation
 *   01-09-2024       Morris              v2.0            added inotify mechanism for monitoring file changes
 *   01-09-2024       Morris              v3.0            replaced inotify with shared memory mechanism
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
#include <sys/shm.h>


#define SHM_KEY 1234                                                 /* Key for shared memory segment */
#define FIFO_PATH "tmp/gps_pipe"                                     /* Path to the named pipe (FIFO) */
#define BUFFER_SIZE 256                                              /* Buffer size for reading/writing data */

volatile sig_atomic_t running = 1;                                   /* Flag to control program execution */

/**
 * signal_handler - Handles SIGINT to allow graceful shutdown.
 *
 * @signum: The signal number received.
 *
 * This function sets the running flag to 0, signaling the main loop to terminate.
 */
void signal_handler(int signum __attribute__((unused)))
{
    /* Placeholder for future logging implementation */
    running = 0;                                                     /* Set flag to stop the main loop */
}

int main()
{
    int         fifo_fd;                                             /* Descriptor for FIFO file */
    char        buffer[BUFFER_SIZE];                                 /* Buffer to hold data */
    ssize_t     bwr;                                                 /* Number of bytes written */
    int         shm_id;                                              /* Shared memory ID */
    DataPacket  *shm_data;                                           /* Pointer to shared memory data */

    /* Get the shared memory segment ID */
    shm_id = shmget(SHM_KEY, sizeof(DataPacket), 0666);
    if (shm_id == -1)
    {
        perror("shmget failed");                                     /* Handle error if shmget fails */
        exit(EXIT_FAILURE);
    }

    /* Attach to the shared memory */
    shm_data = (DataPacket *)shmat(shm_id, NULL, 0);
    if (shm_data == (DataPacket *)-1)
    {
        perror("shmat failed");                                      /* Handle error if shmat fails */
        exit(EXIT_FAILURE);
    }

    /* Check if FIFO exists, create it if not */
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

    /* Main loop: read from shared memory and write to FIFO */
    while (running)
    {
        const char* formatted_data = get_formatted_data(shm_data);   /* Format shared memory data */
        strncpy(buffer, formatted_data, BUFFER_SIZE);                /* Copy formatted data to buffer */
        buffer[BUFFER_SIZE - 1] = '\0';                              /* Ensure the buffer is null-terminated */

        bwr = write(fifo_fd, buffer, strlen(buffer));                /* Write to FIFO */
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
                close(fifo_fd);
                exit(EXIT_FAILURE);
            }
        }
        usleep(10000);                                               /* Wait for 10 milliseconds if no data is available */
    }

    /* Detach from the shared memory */
    if (shmdt(shm_data) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }

    /* Cleanup: close the FIFO */
    close(fifo_fd);

    return 0;
}
