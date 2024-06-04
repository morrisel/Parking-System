/* gcc pipe_server.c -o out_pipe_server */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define FIFO_PATH "tmp/gps_pipe"
#define BUFFER_SIZE 256

volatile sig_atomic_t running = 1;

void signal_handler(int signum)
{
    running = 0;
}

int main()
{
    // Check if the FIFO exists
    if (access(FIFO_PATH, F_OK) == -1)
	{
        // FIFO doesn't exist, create it
        if (mkfifo(FIFO_PATH, 0666) == -1)
		{
            perror("Error creating FIFO");
            exit(EXIT_FAILURE);
        }
    }

	// Open the FIFO for writing (non-blocking and synchronous)
    int fifo_fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK | O_SYNC);
    if (fifo_fd == -1)
	{
        perror("Error opening FIFO");
        exit(EXIT_FAILURE);
    }

    // Read data from a file and send it to the FIFO
    FILE *data_file = fopen("tmp/gps_fifo", "r");
    if (data_file == NULL)
	{
        perror("Error opening data file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, data_file) != NULL && running)
	{
		sleep(1);
        ssize_t bytes_written = write(fifo_fd, buffer, strlen(buffer));
        if (bytes_written == -1)
		{
            if (errno == EAGAIN)
			{
                // FIFO full, try again later
                usleep(10000); // Wait for 10 milliseconds
                continue;
            }
			else
			{
                perror("Error writing to FIFO");
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(data_file);

    // Close the FIFO
    close(fifo_fd);

    return 0;
}

