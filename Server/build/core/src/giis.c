/**
 * giis.c: Reads data from shared memory and writes it to an output file and a FIFO
 *
 * This program reads data from shared memory and writes it to a specified
 * output file as well as a FIFO (First In, First Out) file for further processing.
 * The program ensures that the shared memory is accessed synchronously by using
 * mutexes to avoid data corruption.
 *
 * Compilation:
 *      gcc giis.c -o out_giis
 *
 * Usage:
 *      ./out_giis
 *
 * Features:
 * - Reads data from shared memory defined by SHM_KEY.
 * - Writes data to an output file defined by OUTPUT_FILE.
 * - Writes data to a FIFO file defined by FIFO_TO_DB.
 * - Ensures synchronization using a mutex.
 *
 * Version: v1.0
 * Date:    19-05-2024
 * Author:  Morris
 *                                   
 * Date:            Name:               Version:        Modification:
 *   19-05-2024       Morris              v1.0            created
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>

#include <fcntl.h>                                                   /* open(FIFO_TO_DB, O_WRONLY) */
#include <sys/stat.h>                                                /* mkfifo */

#define BUFFER_SIZE 1024
#define SHM_KEY 0x1234
#define OUTPUT_FILE "giis/gdfs.data"
#define FIFO_TO_DB "giis/ipc_to_db"                                  /* Added named pipe */


/* Shared memory structure */
struct shared_data
{
    char data[BUFFER_SIZE];
};

/* Mutex for shared memory synchronization */
pthread_mutex_t shm_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * read_from_shared_memory - Thread function to read data from shared memory
 *                           and write it to an output file and a FIFO.
 *
 * This function attaches to the shared memory segment specified by SHM_KEY,
 * reads the data from it, and writes the data to an output file defined by OUTPUT_FILE
 * and a FIFO file defined by FIFO_TO_DB. The shared memory data is cleared after
 * being read to prevent duplicate processing.
 *
 * @arg: Unused parameter, required for pthread_create compatibility.
 *
 * Return: NULL
 */
void *read_from_shared_memory(void *arg)
{
    int                 shm_id;
    struct shared_data  *shm_data;
    FILE                *output_file;
    char                last_data[BUFFER_SIZE] = "";

    /* Attach shared memory */
    shm_id = shmget(SHM_KEY, sizeof(struct shared_data), 0666);
    if (shm_id == -1)
    {
        perror("shmget");
        pthread_exit(NULL);
    }
    shm_data = (struct shared_data *)shmat(shm_id, NULL, 0);
    if (shm_data == (void *)-1)
    {
        perror("shmat");
        pthread_exit(NULL);
    }

    /* Open output file for writing */
    output_file = fopen(OUTPUT_FILE, "a");                           /* "a" - append, "w" - write (refresh the file) */
    if (output_file == NULL)
    {
        perror("fopen");
        shmdt(shm_data);
        pthread_exit(NULL);
    }

    /* Open FIFO for writing */
    int fifo_fd = open(FIFO_TO_DB, O_WRONLY);
    if (fifo_fd == -1)
    {
        perror("open fifo");
        fclose(output_file);
        shmdt(shm_data);
        pthread_exit(NULL);
    }

    /* Loop to read data from shared memory and write to the file */
    while (1)
    {
        /* Lock mutex */
        pthread_mutex_lock(&shm_mutex);

        /* Read data from shared memory */
        /* if (strlen(shm_data->data) > 0 && strcmp(shm_data->data, last_data) != 0) */
        if (strlen(shm_data->data) > 0)
        {
            /* Write data to file */
            fprintf(output_file, "%s\n", shm_data->data);
            fflush(output_file);

            /* Write data to FIFO */
            write(fifo_fd, shm_data->data, strlen(shm_data->data));

            /* Update last_data with the current data */
            strncpy(last_data, shm_data->data, BUFFER_SIZE);


            /* Clear shared memory data */
            memset(shm_data->data, 0, BUFFER_SIZE);
        }

        /* Unlock mutex */
        pthread_mutex_unlock(&shm_mutex);

        /* Sleep for a while to avoid busy waiting */
        usleep(100000);                                              /* 100ms */
    }

    /* Cleanup */
    close(fifo_fd);
    fclose(output_file);
    shmdt(shm_data);

    pthread_exit(NULL);
}

int main()
{
    pthread_t thread_id;

    /* Create FIFO if it doesn't exist */
    if (access(FIFO_TO_DB, F_OK) == -1)
    {
        if (mkfifo(FIFO_TO_DB, 0666) == -1)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }

    /* Create a thread to read from shared memory */
    if (pthread_create(&thread_id, NULL, read_from_shared_memory, NULL) != 0)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    /* Wait for the thread to finish */
    pthread_join(thread_id, NULL);

    return 0;
}

