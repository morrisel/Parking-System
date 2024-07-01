/**
 * insert_data_from_giis_shm.c: Read data from shared memory and insert into SQLite database
 *
 * This program reads data from a shared memory segment (using FIFO) and inserts it into
 * an SQLite database. The data is expected to be in a specific format and is parsed
 * accordingly before being inserted into the database.
 *
 * Compilation:
 *   gcc insert_data_from_giis_shm.c -o out_insert_data_from_giis_shm
 *
 * Usage:
 *   ./out_insert_data_from_giis_shm
 *
 * Features:
 * - Reads data from a named FIFO defined by FIFO_TO_DB.
 * - Parses the data and inserts it into an SQLite database defined by DB_PATH.
 * - Handles errors during file operations and SQLite command execution.
 *
 * Version: v1.0
 * Date:    01-06-2024
 * Author:  Morris
 *                                   
 * Date:            Name:               Version:        Modification:
 *   01-06-2024       Morris              v1.0            created
 *
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>

#define DATA_FILE "giis/gdfs.data"                            /* Path to the data file */
#define DB_PATH "prksys_db.db"                               /* Path to the SQLite database */
#define FIFO_TO_DB "giis/ipc_to_db"                           /* Named FIFO path */


/**
 * process_line - Process a single line of input data
 * @line: The line of input data to be processed
 *
 * This function parses a line of input data to extract the mac address,
 * status, and coordinates (x, y, z). It then constructs an SQLite command
 * to insert this data into the database and executes the command.
 *
 * Return: void
 */
void process_line(const char *line)
{
    char    mac_address[18];          /* Buffer for MAC address */
    char    status;                   /* Status character */
    double  x, y, z;                  /* Coordinates */

	/* Parse the line to extract the data */
    if (sscanf(line, "%17s: %c: x %lf y %lf z %lf", mac_address, &status, &x, &y, &z) != 5)
	{
        fprintf(stderr, "Error parsing line: %s\n", line);
        return;
    }

    /* Construct the SQLite command to insert the data */
    char    command[256];
    snprintf(command, sizeof(command), "sqlite %s \"INSERT INTO Customer_Data (mac_address, status, x, y, z) VALUES ('%s', '%c', %.2f, %.2f, %.2f);\"", DB_PATH, mac_address, status, x, y, z);

	/* Execute the SQLite command */
    int result = system(command);
    if (result != 0)
	{
        fprintf(stderr, "Error executing SQLite command: %s\n", command);
    }
}

/**
 * process_data_file - Process the data file
 *
 * This function opens the data file for reading, locks it to prevent
 * simultaneous access by other processes, and reads each line to process
 * it using the process_line function. The file is unlocked and closed
 * after processing.
 *
 * Return: void
 */
void process_data_file()
{
	/* Open the data file for reading */
    int     fd = open(DATA_FILE, O_RDONLY);
    if (fd == -1)
	{
        perror("Error opening file");
        return;
    }

    /* Lock the file to prevent other processes from accessing it simultaneously */
    if (flock(fd, LOCK_EX | LOCK_NB) == -1)
	{
        perror("Error locking file");
        close(fd);
        return;
    }

	/* Convert the file descriptor to a file pointer */
    FILE *file = fdopen(fd, "r");
    if (file == NULL)
	{
        perror("Error opening file");
        close(fd);
        return;
    }

	/* Read and process each line of the file */
    char line[256];
    while (fgets(line, sizeof(line), file))
	{
        process_line(line);
    }

	/* <B1: explicitly unlock */
    /* Unlock the file */
    if (flock(fd, LOCK_UN) == -1)
	{
        perror("Error unlocking file");
    }
	/* B1> */

	/* Close the file and release the lock */
    fclose(file);
    close(fd);
}

/**
 * process_fifo - Process data from the FIFO
 *
 * This function opens the named FIFO for reading and continuously reads data
 * from it. Each line of data read is processed using the process_line function.
 * If the FIFO is closed (EOF is reached), it is reopened to wait for new data.
 *
 * Return: void
 */
void process_fifo()
{
	/* Open the FIFO for reading */
    int fd = open(FIFO_TO_DB, O_RDONLY);
    if (fd == -1)
	{
		perror("Error opening FIFO");
        return;
    }

    char line[256];
    while (1)
	{
		/* Read data from the FIFO */
        ssize_t bytes_read = read(fd, line, sizeof(line) - 1);
        if (bytes_read > 0)
		{
			/* Null-terminate the line and process it */
            line[bytes_read] = '\0';
            process_line(line);
        }
		else if (bytes_read == 0)
		{
			/* End of file, close and reopen to wait for new data */
            close(fd);
            fd = open(FIFO_TO_DB, O_RDONLY);
            if (fd == -1)
			{
				perror("Error opening FIFO");
                return;
            }
        }
    }
}

/**
 * main - Entry point of the program
 *
 * This function creates the named FIFO if it doesn't exist, processes data from
 * the data file (optional, can be enabled by uncommenting the corresponding line),
 * and then processes data from the FIFO.
 *
 * Return: 0 on success, exits with failure code otherwise
 */
int main()
{
    /* Create FIFO if it doesn't exist */
    if (access(FIFO_TO_DB, F_OK) == -1)
	{
        if (mkfifo(FIFO_TO_DB, 0666) == -1)
		{
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }

	/* Process data from the file */
    printf("Processing data file.\n");
    /* process_data_file(); // Uncomment if you want to process the data file as well */


    /* Process data from the FIFO */
    printf("Waiting for data from FIFO...\n");
    process_fifo();
    printf("FIFO data processed.\n");

    return 0;
}

