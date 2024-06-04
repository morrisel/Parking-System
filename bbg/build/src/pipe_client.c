/* gcc pipe_client.c -o out_pipe_client */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define FIFO_PATH "tmp/gps_pipe"
#define BUFFER_SIZE 1024

int main()
{
	// Create and open the FIFO for reading
	int fifo_fd = open(FIFO_PATH, O_RDONLY);
	if (fifo_fd == -1)
	{
		perror("Error opening FIFO");
		exit(EXIT_FAILURE);
	}

	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;

	while (1)
	{
		bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1); // Leave space for null terminator
		if (bytes_read > 0)
		{
			sleep(1);
			buffer[bytes_read] = '\0'; // Null-terminate the string
			sleep(1);
			printf("%s", buffer);
			fflush(stdout); // Flush output buffer
		} 
		else if (bytes_read == 0)
		{
			sleep(1);
			// End of file
			break;
		}
		else if (bytes_read == -1 && errno != EAGAIN)
		{
			perror("Error reading from FIFO");
			exit(EXIT_FAILURE);
		}
	}

	close(fifo_fd);

	return 0;
}

