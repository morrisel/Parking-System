/** 
 * data_formatter.c
 *
 * This module provides functionality for processing and formatting
 * lines of received data into a more readable and structured format.
 * The formatted data is then written to an output file.
 *
 * The expected format of the input data is a fixed-length string,
 * where each segment of the string represents a pair of coordinates
 * and a unique identifier.
 *
 * Version: v1.0
 * Date:    01-09-2024
 * Author:  Morris
 *
 * Date:            Name:            Version:    Modification:
 * 04-09-2024         morris        v1.0        created
 *
 *
 */


#include "data_formatter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define LINE_LENGTH 13
#define OUTPUT_FILE "ext_data.txt"

/**
 * format_data_line
 *
 * This function takes a single line of input data, extracts the relevant
 * numeric components, and writes them to the output file in a formatted style.
 *
 * Parameters:
 *   @line: A string representing a single line of input data.
 *   @file: A file pointer to which the formatted data will be written.
 *
 * Return: void
 *
 * This function assumes that the input line has a fixed format and length.
 * If the line length is incorrect, an error message is printed, and the
 * function returns without processing the line.
 */
void format_data_line(const char *line, FILE *file)
{
    if (strlen(line) != LINE_LENGTH)   /* Ensure the line is of the expected length */
    {
        fprintf(stderr, "Invalid line length: %s\n", line);
        return;
    }

    /*
     * Variables for storing parsed numeric values.
     * The input data is expected to be in pairs of digits,
     * representing coordinates for x, y, and z.
     */
    int x1, x2, y1, y2, z1, z2;

    /* Extract and convert the character digits to integers */
    x1 = (line[1]  - '0') * 10 + (line[2]  - '0');
    x2 = (line[3]  - '0') * 10 + (line[4]  - '0');
    y1 = (line[5]  - '0') * 10 + (line[6]  - '0');
    y2 = (line[7]  - '0') * 10 + (line[8]  - '0');
    z1 = (line[9]  - '0') * 10 + (line[10] - '0');
    z2 = (line[11] - '0') * 10 + (line[12] - '0');

    /*
     * Write the formatted data to the output file.
     * The format includes the first character from the input line,
     * followed by the x, y, and z coordinates in a structured format.
     */
    fprintf(file, "%c: x %02d.%02d y %02d.%02d z %02d.%02d\n",
            line[0], x1, x2, y1, y2, z1, z2);

}

/**
 * process_received_message
 *
 * This function processes the entire received message by breaking it down
 * into individual lines, each of which is then formatted and written to the
 * output file.
 *
 * Parameters:
 *   @received_message: The complete message string that needs processing.
 *
 * Return: void
 *
 * The function opens the output file, iterates over the message string in
 * chunks of LINE_LENGTH, and calls `format_data_line` to format and write
 * each chunk. The output file is closed once all lines are processed.
 */
void process_received_message(const char *received_message)
{
    FILE *file = fopen(OUTPUT_FILE, "w"); /* Open the output file for writing */
    if (!file)
    {
        perror("Failed to open output file");
        return;
    }

    size_t len = strlen(received_message);
    char line[LINE_LENGTH + 1]; /* Temporary storage for each line */

    /* Iterate over the message, processing each line */
    for (size_t i = 0; i < len; i += LINE_LENGTH)
    {
        strncpy(line, &received_message[i], LINE_LENGTH);
        line[LINE_LENGTH] = '\0'; /* Ensure the line is null-terminated */

        /* Format and write the line to the output file */
        format_data_line(line, file);
    }

    fclose(file);/* Close the output file */
}

