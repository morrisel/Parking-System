/* gcc tcp_client.c -o out_tcp_client */

#include <stdio.h>         /* Standard I/O library */
#include <stdlib.h>        /* Standard library for general functions */
#include <string.h>        /* String handling functions */
#include <unistd.h>        /* POSIX API */
#include <sys/types.h>     /* Data types used in system calls */
#include <sys/stat.h>      /* File status and permission functions */
#include <fcntl.h>         /* File control options */
#include <errno.h>         /* Error number definitions */
#include <arpa/inet.h>     /* Definitions for internet operations */

#define FIFO_PATH   "tmp/gps_pipe"   /* Path to the FIFO file */
#define SERVER_PORT 12345            /* Server port number */
#define SERVER_IP   "192.168.15.188" /* Server IP address */
#define BUFFER_SIZE 1024             /* Buffer size for reading and writing data */

int main()
{
    int                 fifo_fd;              /* Descriptor for FIFO file */
    int                 sock;                 /* Descriptor for the socket */
    struct sockaddr_in  server_addr;          /* Structure to store server address */
    char                buffer[BUFFER_SIZE];  /* Buffer to hold data */
    ssize_t             bytes_read;           /* Number of bytes read */

    /* Open the FIFO in read-only mode */
    fifo_fd = open(FIFO_PATH, O_RDONLY);
    if (fifo_fd == -1)
    {
        perror("Error opening FIFO");
        exit(EXIT_FAILURE);
    }

    /* Create a TCP socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Initialize the server address structure */
    server_addr.sin_family = AF_INET;                   /* Use IPv4 address family */
    server_addr.sin_port = htons(SERVER_PORT);          /* Set server port, converting to network byte order */
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP); /* Set server IP address */

    /* Connect to the server */
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    /* Read from the FIFO and send data to the server */
    while (1)
    {
        bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1); /* Leave space for null-terminator */
        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';            /* Null-terminate the string */
            printf("Read from FIFO: %s", buffer); /* Print the read data */
            fflush(stdout);                       /* Flush the output buffer */

            /* Send data to the server */
            if (send(sock, buffer, strlen(buffer), 0) < 0)
            {
                perror("send");
                exit(EXIT_FAILURE);
            }
            printf("Sent to server: %s", buffer); /* Print the sent data */
        }
        else if (bytes_read == 0)
        {
            break;  /* End of data in FIFO */
        }
        else if (bytes_read == -1 && errno != EAGAIN)
        {
            perror("Error reading from FIFO");
            exit(EXIT_FAILURE);
        }
    }

    /* Close the FIFO and socket */
    close(fifo_fd);
    close(sock);

    return 0;
}

