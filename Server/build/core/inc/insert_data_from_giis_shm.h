#ifndef INSERT_DATA_FROM_GIIS_SHM_H
#define INSERT_DATA_FROM_GIIS_SHM_H


#define DATA_FILE "giis/gdfs.data"                                   /* Path to the data file */
#define DB_PATH "prksys_db.db"                                       /* Path to the SQLite database */
#define FIFO_TO_DB "giis/ipc_to_db"                                  /* Named FIFO path */

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
void process_line(const char *line);


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
void process_data_file();


/**
 * process_fifo - Process data from the FIFO
 *
 * This function opens the named FIFO for reading and continuously reads data
 * from it. Each line of data read is processed using the process_line function.
 * If the FIFO is closed (EOF is reached), it is reopened to wait for new data.
 *
 * Return: void
 */
void process_fifo();


#endif  /* INSERT_DATA_FROM_GIIS_SHM_H */
