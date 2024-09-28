#ifndef DATA_STRUCT_FORMAT_H
#define DATA_STRUCT_FORMAT_H

#include <stdio.h>
#include <stdint.h>                         /* for uint16_t */

/* Define constants for data handling */
#define MAX_DATA_LENGTH 50
#define SHM_KEY 1234
#define SHM_SIZE sizeof(DataPacket)


/**
 * DataPacket
 * This structure represents a data packet with an operation code and
 * three coordinate values (x, y, z).
 */
#pragma pack(push, 1)
struct __attribute__((packed)) DataPacket
{
    char op_code;    /* Operation code: 'D' for dynamic data, 'S' for static data */
    uint16_t x;     /* X coordinate */
    uint16_t y;     /* Y coordinate */
    uint16_t z;     /* Z coordinate */
};
#pragma pack(pop)
#define DataPacket struct DataPacket


/**
 * format_data_struct
 *
 * Formats the contents of a DataPacket structure into a human-readable string.
 *
 * Parameters:
 *   @packet: Pointer to a DataPacket structure to be formatted.
 *
 * Return: void
 */
void format_data_struct(DataPacket *packet);


/**
  * get_formatted_data
  *
  * Copies the given DataPacket into shared memory and returns the most
  * recently formatted data as a string.
  *
  * Parameters:
  *   @packet: Pointer to a DataPacket structure.
  *
  * Return: A pointer to the formatted string.
  */
const char *get_formatted_data(DataPacket *packet);


/**
 * remove_shared_memory
 *
 * Removes the shared memory segment. Should be called before exiting the program.
 *
 * Return: void
 */
void remove_shared_memory();

#endif  /* DATA_STRUCT_FORMAT_H */
