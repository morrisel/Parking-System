/**
 * ipc_sender.c: Sends data from a file to a FIFO (named pipe)
 *
 * This program reads GPS data from a specified file and writes it to a
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
 * - Reads from a GPS data file located at 'ext_data.txt'.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/inotify.h>

#define FIFO_PATH "tmp/gps_pipe"                                     /* Path to the named pipe (FIFO) */
#define BUFFER_SIZE 256                                              /* Buffer size for reading/writing data */

#define FILE_TO_MONITOR "ext_data.txt"                               /* File to monitor for changes */
#define EVENT_SIZE  ( sizeof (struct inotify_event) )                /* Size of one event */
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )                   /* Buffer length */

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

int main()
{
    int      fifo_fd;                                                /* Descriptor for FIFO file */
    char     buffer[BUFFER_SIZE];                                    /* Buffer to hold data */
    ssize_t  bwr;                                                    /* Number of bytes written */
    FILE     *dfile;                                                 /* File pointer for the data file */
    int      inotify_fd, watch_fd;                                   /* Descriptors for inotify */
    char     event_buf[BUF_LEN];                                     /* Buffer for inotify events */

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

    /* Initialize inotify */
    inotify_fd = inotify_init();
    if (inotify_fd < 0)
    {
        perror("inotify_init");
        close(fifo_fd);
        exit(EXIT_FAILURE);
    }

    /* Add watch on the file */
    watch_fd = inotify_add_watch(inotify_fd, FILE_TO_MONITOR, IN_CLOSE_WRITE);
    if (watch_fd < 0)
    {
        perror("inotify_add_watch");
        close(inotify_fd);
        close(fifo_fd);
        exit(EXIT_FAILURE);
    }

    /* Monitor the file for changes and write to FIFO when it's modified */
    while (running)
    {
        int length = read(inotify_fd, event_buf, BUF_LEN);
        if (length < 0)
        {
            perror("read");
            break;
        }

        int i = 0;
        while (i < length)
        {
            struct inotify_event *event = (struct inotify_event *) &event_buf[i];
            if (event->mask & IN_CLOSE_WRITE)
            {
                /* File was modified and closed, read and send data */
                dfile = fopen(FILE_TO_MONITOR, "r");
                if (dfile == NULL)
                {
                    perror("Error opening data file");
                    break;
                }

                while (fgets(buffer, BUFFER_SIZE, dfile) != NULL)
                {
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
                            fclose(dfile);
                            close(inotify_fd);
                            close(fifo_fd);
                            exit(EXIT_FAILURE);
                        }
                    }
                }

                fclose(dfile);                                       /* Close the data file */
            }
            i += EVENT_SIZE + event->len;
        }
    }

    /* Cleanup */
    inotify_rm_watch(inotify_fd, watch_fd);
    close(inotify_fd);
    close(fifo_fd);

    return 0;
}

