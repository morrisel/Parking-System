/* gcc bbg_gps_sender.c -o out_bbg_gps_sender */
// bbg_gps_sender.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345
#define SERVER_IP "192.168.1.100"
#define BUFFER_SIZE 1024

int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    FILE *gps_fifo;

    // Open the gps_fifo file
    gps_fifo = fopen("tmp/gps_fifo", "r");
    if (gps_fifo == NULL)
	{
        perror("fopen");
        exit(EXIT_FAILURE);
    }

	// Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	// Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
	{
        perror("connect");
        exit(EXIT_FAILURE);
    }

	// Read lines from gps_fifo and send them to the server
    while (fgets(buffer, BUFFER_SIZE, gps_fifo) != NULL) 
	{
        // Send the line to the server
        send(sock, buffer, strlen(buffer), 0);
        // Print the sent message to the terminal
        printf("Sent: %s", buffer);
    }

    // Close the file and socket
    fclose(gps_fifo);
    close(sock);

    return 0;
}

