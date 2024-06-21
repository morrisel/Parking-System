#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

#define FIFO_PATH   "tmp/gps_pipe"   /* Path to the FIFO file */
#define SERVER_PORT 12345            /* Server port number */
#define BUFFER_SIZE 1024             /* Buffer size for reading and writing data */

void get_server_ip(char *ip_buffer, size_t buffer_size)
{
    FILE *fp;
    char cmd[] = "hostname -I | awk '{print $1}'";

    fp = popen(cmd, "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    if (fgets(ip_buffer, buffer_size, fp) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    // Remove trailing newline character
    size_t len = strlen(ip_buffer);
    if (len > 0 && ip_buffer[len-1] == '\n') {
        ip_buffer[len-1] = '\0';
    }

    pclose(fp);
}

int main()
{
    int fifo_fd;              /* Descriptor for FIFO file */
    int sock;                 /* Descriptor for the socket */
    struct sockaddr_in saddr; /* Structure to store server address */
    char buffer[BUFFER_SIZE]; /* Buffer to hold data */
    ssize_t brd;              /* Number of bytes read */
    char server_ip[INET_ADDRSTRLEN];  /* Buffer to hold the server IP address */

    // Get the server IP address at runtime
    get_server_ip(server_ip, sizeof(server_ip));

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
    saddr.sin_family = AF_INET;                   /* Use IPv4 address family */
    saddr.sin_port = htons(SERVER_PORT);          /* Set server port, converting to network byte order */
    if (inet_pton(AF_INET, server_ip, &saddr.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    /* Connect to the server */
    if (connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    /* Read from the FIFO and send data to the server */
    while (1)
    {
        brd = read(fifo_fd, buffer, sizeof(buffer) - 1); /* Leave space for null-terminator */
        if (brd > 0)
        {
            buffer[brd] = '\0';            /* Null-terminate the string */
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
        else if (brd == 0)
        {
            break;  /* End of data in FIFO */
        }
        else if (brd == -1 && errno != EAGAIN)
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

