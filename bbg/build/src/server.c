/* gcc server.c -o out_server */
/* server.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int main()
{
	int                  ssck;
	int                  csck;
	struct  sockaddr_in  saddr;
	struct  sockaddr_in  caddr;
	char                 buffer[BUFFER_SIZE + 1];   /* +1 for adding '\0' */
	socklen_t            caddrlen = sizeof(caddr);

	int                  tbrecv;	     /* Total bytes received from client */
	int                  brecv;
	char                 *line;   /* Split the buffer into lines */


	/* Create a socket */
	if ((ssck = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(SERVER_PORT);
	saddr.sin_addr.s_addr = INADDR_ANY;

	/* Bind the socket to the port */
	if (bind(ssck, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	/* Listen for incoming connections */
	if (listen(ssck, 5) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	printf("Server is listening on port %d\n", SERVER_PORT);

	while (1)
	{
		tbrecv = 0;

		/* Accept a connection from a client */
		csck = accept(ssck, (struct sockaddr *)&caddr, &caddrlen);
		if (csck < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}

		/* Read data from the client and print it to the terminal */
		while ((brecv = recv(csck, buffer, BUFFER_SIZE, 0)) > 0)
		{
			buffer[brecv] = '\0';       /* Null-terminate the string */
			line = strtok(buffer, "\n");       /* Tokenize within the recv loop */
			while (line != NULL)
			{
				printf("Received: %s\n", line);  /* Print each line */
				line = strtok(NULL, "\n");       /* Move to the next line */
			}
			tbrecv += brecv;
		}

		/* Print a message indicating the end of data reception from the client */
		if (tbrecv > 0)
		{
			printf("Complete message received from client.\n");
		}

		/* Close the client socket */
		close(csck);
	}

	/* Close the server socket */
	close(ssck);

	return 0;
}

