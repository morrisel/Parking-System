#ifndef GIIS_H
#define GIIS_H

#include <pthread.h>

/* Constants */
#define BUFFER_SIZE 1024
#define SHM_KEY 0x1234
#define OUTPUT_FILE "giis/gdfs.data"
#define FIFO_TO_DB "giis/ipc_to_db"                                  /* Added named pipe */


/* Structure for shared memory */
struct shared_data
{
    char data[BUFFER_SIZE];
};

/* External global variables */
extern pthread_mutex_t shm_mutex;

/* Function declarations */

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
void *read_from_shared_memory(void *arg);


#endif /* GIIS_H */

