/** data_struct_format.c */

#include "data_struct_format.h"
/* <A1: Includes for shared memory operations ---- */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
/* A1> */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Global variables for shared memory */
int shm_id;
DataPacket *shm_data;

/* Static buffer to hold formatted data */
static char formatted_data[MAX_DATA_LENGTH];

void format_data_struct(DataPacket *packet)
{
    snprintf(formatted_data, sizeof(formatted_data), "%c: x %.2f y %.2f z %.2f",
            packet->op_code,
            packet->x / 100.0,
            packet->y / 100.0,
            packet->z / 100.0);
}

const char *get_formatted_data()
{
    /* Create the segment of the shared memory */
    shm_id = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shm_id == -1)
    {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    /* Attach the sharing memory */
    shm_data = (DataPacket *)shmat(shm_id, NULL, 0);
    if (shm_data == (DataPacket *)-1)
    {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
    
    /* Initialize the shared memory to zero */
    memset(shm_data, 0, sizeof(DataPacket));

    /* Format the data structure */
    format_data_struct(shm_data);

    /* Detach the shared memory after using it */
    if (shmdt(shm_data) == -1)
    {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }

    return formatted_data;
}

/* Remove the shared memory segment */
void remove_shared_memory()
{
    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
        perror("shmctl failed");
        exit(EXIT_FAILURE);
    }
}
