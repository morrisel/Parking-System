/* gcc server.c -o out_server */
// server.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int main() 
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE + 1];  // +1 for adding '\0'
    socklen_t client_addr_len = sizeof(client_addr);

    // Create a socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the port
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
	{
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) < 0)
	{
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", SERVER_PORT);

    while (1)
	{
        // Read data from the client and print it to the terminal
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0)
		{
            perror("accept");
            exit(EXIT_FAILURE);
        }

        int total_bytes_received = 0;
        int bytes_received;

        // Read data from the client and print it to the terminal
        while ((bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0)) > 0)
		{
            buffer[bytes_received] = '\0';   // Null-terminate the string with '\0'
            char *line = strtok(buffer, "\n");  // Split the buffer into lines
            while (line != NULL)
			{
                printf("Received: %s\n", line);  // Print each line
                line = strtok(NULL, "\n");  // Move to the next line
            }
            total_bytes_received += bytes_received;
        }

		// Print a message indicating the end of data reception from the client
        if (total_bytes_received > 0)
		{
            printf("Complete message received from client.\n");
        }

        // Close the client socket
        close(client_sock);
    }

    // Close the server socket
    close(server_sock);

    return 0;
}

