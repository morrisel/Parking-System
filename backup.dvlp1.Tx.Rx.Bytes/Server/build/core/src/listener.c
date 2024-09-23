/**
 * listener.c: Program to listen for data in shared memory and notify via FIFO
 *
 * This program reads data from shared memory and checks for changes. When data
 * changes are detected, it writes a notification to a named FIFO file.
 * The program also handles clean termination on receiving a SIGINT signal.
 *
 * Compilation:
 *      gcc listener.c -o out_listener
 *
 * Usage:
 *      ./out_listener
 *
 * Features:
 * - Reads data from shared memory segment defined by SHM_KEY.
 * - Monitors the data for changes.
 * - Sends notifications to a FIFO defined by FIFO_NAME when data changes.
 * - Handles termination signals to clean up resources.
 *
 * Version: v1.0
 * Date:    20-05-2024
 * Author:  Morris
 *                                   
 * Date:            Name:               Version:        Modification:
 *   20-05-2024       Morris              v1.0            created
 *
 */


#include "../inc/listener.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <signal.h>


volatile sig_atomic_t running = 1;                                   /* Flag for running status */

/**
 * signal_handler - Signal handler to set the running flag to 0.
 */
void signal_handler(int signum)
{
    running = 0;
}

int main()
{
    /* Get shared memory ID */
    int shm_id = shmget(SHM_KEY, sizeof(struct shared_data), 0666);
    if (shm_id == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    /* Attach shared memory segment */
    struct shared_data *shm_data = (struct shared_data *)shmat(shm_id, NULL, 0);
    if (shm_data == (void *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    /* Check if FIFO exists */
    if (access(FIFO_NAME, F_OK) == -1)
    {
        /* Create FIFO if it doesn't exist */
        if (mkfifo(FIFO_NAME, 0666) == -1)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }

    /* Set up signal handler for SIGINT */
    signal(SIGINT, signal_handler);

    char last_data[BUFFER_SIZE] = "";                                /* Buffer to store last read data */
    while (running)
    {
        sleep(10);                                                   /* Sleep for 10 seconds */

        /* Check if data has changed */
        if (strcmp(last_data, shm_data->data) != 0)
        {
            strcpy(last_data, shm_data->data);                       /* Update last data */

            /* Write notification to FIFO */
            int fd = open(FIFO_NAME, O_WRONLY);
            if (fd == -1)
            {
                perror("open");
                exit(EXIT_FAILURE);
            }
            write(fd, "data received\n", 15);                        /* Write notification */
            close(fd);
        }
    }

    /* Detach shared memory segment */
    shmdt(shm_data);
    unlink(FIFO_NAME);                                               /* Remove the FIFO file */

    puts("");
    return 0;
}

