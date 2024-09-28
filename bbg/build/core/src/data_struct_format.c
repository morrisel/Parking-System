/**
 * data_struct_format.c: Formatting and shared memory handling for data packets
 *
 * This file implements functions that format `DataPacket` structures and
 * manage shared memory segments for inter-process communication (IPC). 
 * It includes functions to attach, detach, and remove shared memory segments,
 * as well as format `DataPacket` data into a human-readable string.
 *
 * Compilation:
 *      gcc data_struct_format.c -o out_data_struct_format -lrt
 *
 * Usage:
 *      #include "data_struct_format.h"
 *      DataPacket packet = { 'D', 100, 200, 300 };
 *      const char* formatted_data = get_formatted_data(&packet);
 *
 * Version: v1.0
 * Date:    01-09-2024
 * Author:  Morris
 *
 * Date:            Name:               Version:        Modification:
 *   01-09-2024       Morris              v1.0            created
 */

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
int shm_id;   /* Shared memory ID */
DataPacket *shm_data;  /* Pointer to shared memory */

/* Static buffer to hold formatted data */
static char formatted_data[MAX_DATA_LENGTH];

/**
 * format_data_struct - Formats a DataPacket into a human-readable string.
 *
 * @packet: Pointer to the DataPacket structure to be formatted.
 *
 * This function converts the `DataPacket` fields into a string that contains
 * the operation code followed by the x, y, and z coordinates formatted as
 * floating-point numbers.
 */
void format_data_struct(DataPacket *packet)
{
    snprintf(formatted_data, sizeof(formatted_data), "%c: x %.2f y %.2f z %.2f",
            packet->op_code,
            packet->x / 100.0,
            packet->y / 100.0,
            packet->z / 100.0);
}


/**
 * get_formatted_data - Copies data into shared memory and returns formatted string.
 *
 * @packet: Pointer to the DataPacket structure to be copied into shared memory.
 *
 * This function attaches to the shared memory, copies the `DataPacket` data
 * into it, and then returns a formatted string based on the data.
 *
 * Return: A pointer to the formatted string.
 */
const char *get_formatted_data(DataPacket *packet)
{
    /* Create the segment of the shared memory segment */
    shm_id = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shm_id == -1)
    {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    /* Attach to the shared memory segment */
    shm_data = (DataPacket *)shmat(shm_id, NULL, 0);
    if (shm_data == (DataPacket *)-1)
    {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    /* Copy packet data into shared memory */
    memcpy(shm_data, packet, sizeof(DataPacket));
    /* Initialize the shared memory to zero */
    //memset(shm_data, packet, sizeof(DataPacket));
    //memset(shm_data, 0, sizeof(DataPacket));


    /* Format the data into a readable string */
    format_data_struct(packet);
    //format_data_struct(shm_data);

    /* Detach the shared memory after using it */
    //    if (shmdt(shm_data) == -1)
    //    {
    //        perror("shmdt failed");
    //        exit(EXIT_FAILURE);
    //    }

    return formatted_data;
}


/**
 * remove_shared_memory - Removes the shared memory segment.
 *
 * This function detaches from and removes the shared memory segment identified
 * by `shm_id`. It is typically called before the program exits to free resources.
 */
void remove_shared_memory()
{
    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
        perror("shmctl failed");
        exit(EXIT_FAILURE);
    }
}
