/**
 * tcp_client.c: Client to read from FIFO and send data to a TCP server
 *
 * This program reads data from a named FIFO (First In, First Out) file
 * and sends it to a specified TCP server. The program dynamically
 * retrieves the IP and MAC addresses of the client machine for
 * identification purposes.
 *
 * Compilation:
 *      gcc tcp_client.c -o out_tcp_client
 *
 * Usage:
 *      ./out_tcp_client
 *
 * Features:
 * - Reads from a FIFO file defined by FIFO_PATH.
 * - Connects to a TCP server defined by SERVER_IP and SERVER_PORT.
 * - Retrieves and displays the client's IP and MAC addresses.
 * - Reads data from the FIFO, prepends the MAC address, and sends it to the server.
 * 
 * Version: v1.0
 * Date:    26-03-2024
 * Author:  Morris
 *
 * Date:            Name:               Version:        Modification:
 *   26-03-2024       Morris              v1.0            created
 *
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* <A1: Include headers for network interface functions (headers for getting IP and MAC addresses) */
#include <sys/ioctl.h>	                       /*  ioctl  */
#include <sys/socket.h>                        /*  SIOCGIFADDR  */
#include <net/if.h>                            /*  struct ifreq,  IFNAMSIZ  */
/* A1:> */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

#define FIFO_PATH   "tmp/gps_pipe"             /* Path to the FIFO file */
#define SERVER_PORT 12345                      /* Server port number */
#define BUFFER_SIZE 1024                       /* Buffer size for reading and writing data */
/* #define SERVER_IP   "192.168.15.188" */          /* Server IP address */
#define SERVER_IP   "192.168.7.1"            /* Adjust based on BBG interfaces */

/* #define INTERFACE_PREFIX "ens" */                 /* Network interface based on VM */
#define INTERFACE_PREFIX "usb"               /* Adjust based on BBG interfaces */
#define MAX_INTERFACE_NUMBER 40


/**
 * get_ip_address - Retrieves the IP address of the first active network interface
 *                  matching a given prefix.
 *
 * This function iterates through network interfaces with a specified prefix,
 * attempting to retrieve their IP addresses. The prefix is defined by the macro
 * INTERFACE_PREFIX. The function tries up to MAX_INTERFACE_NUMBER interfaces,
 * and if a valid IP address is found, it is copied to the provided buffer.
 * 
 * @ip_buffer: A buffer to store the retrieved IP address as a string.
 * @buffer_size: The size of the ip_buffer.
 *
 * Return: 0 on success, with ip_buffer containing the IP address.
 *        -1 on failure, with an error message printed to stderr.
 *
 * Note: The function assumes that the provided buffer has sufficient size to
 *       hold the IP address in dotted-decimal notation (INET_ADDRSTRLEN).
 */
int get_ip_address(char *ip_buffer, size_t buffer_size)
{
	char  iface_name[IFNAMSIZ];                 /* Buffer for interface name */
	char  ip_address[INET_ADDRSTRLEN];          /* Buffer for temporary IP storage */
	int   found = 0;                            /* Flag to indicate if an IP was found */

	/* Iterate through potential interface names */
	for (int i = 0; i <= MAX_INTERFACE_NUMBER; i++)
	{
		/* Attempt to get the IP address for the current interface */
		int fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (fd == -1)
		{
			perror("socket");                  /* Check for socket creation error */
			continue;                          /* Skip to the next interface */
		}

		/* Construct interface name */
		snprintf(iface_name, IFNAMSIZ, "%s%d", INTERFACE_PREFIX, i);

		struct ifreq ifr;
		memset(&ifr, 0, sizeof(struct ifreq));             /* Zero out the ifreq structure */
		strncpy(ifr.ifr_name, iface_name, IFNAMSIZ-1);     /* Copy interface name to ifreq */

		/* Use ioctl to get the interface address */
		if (ioctl(fd, SIOCGIFADDR, &ifr) == -1)
		{
			close(fd);
			continue;                          /* Skip to the next interface */
		}

		struct sockaddr_in  *sa  = (struct sockaddr_in *)&ifr.ifr_addr;
		const char          *ip  = inet_ntoa(sa->sin_addr);          /* Convert to dotted-decimal notation */

		strncpy(ip_address, ip, sizeof(ip_address));       /* Store IP temporarily */

		close(fd);                             /* Close the socket */

		/* If IP address is obtained successfully, copy it to buffer and exit loop */
		strncpy(ip_buffer, ip_address, buffer_size);
		found = 1;
		break;
	}

	if (!found)
	{
		fprintf(stderr, "No active interface found with prefix %s\n", INTERFACE_PREFIX);
		return -1;  /* Return an error if no active interface was found */
	}

	return 0; /* Return success */
}



/**
 * get_mac_address - Retrieves the MAC address of the first active network interface
 *                   matching a given prefix.
 *
 * This function iterates through network interfaces with a specified prefix,
 * attempting to retrieve their MAC addresses. The prefix is defined by the macro
 * INTERFACE_PREFIX. The function tries up to MAX_INTERFACE_NUMBER interfaces,
 * and if a valid MAC address is found, it is copied to the provided buffer.
 *
 * @mac_buffer: A buffer to store the retrieved MAC address as a string.
 * @buffer_size: The size of the mac_buffer.
 *
 * Return: 0 on success, with mac_buffer containing the MAC address.
 *        -1 on failure, with an error message printed to stderr.
 *
 * Note: The function assumes that the provided buffer has sufficient size to
 *       hold the MAC address in standard colon-separated format (17 characters).
 */
int get_mac_address(char *mac_buffer, size_t buffer_size)
{
	char  iface_name[IFNAMSIZ];                 /* Buffer for interface name */
	char  mac_address[18];                      /* Buffer for temporary MAC storage */
	int   found = 0;                             /* Flag to indicate if a MAC was found */

	/* Iterate through potential interface names */
	for (int i = 0; i <= MAX_INTERFACE_NUMBER; i++)
	{
		/* Attempt to get the MAC address for the current interface */
		int fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (fd == -1)
		{
			perror("socket");                  /* Check for socket creation error */
			continue;                          /* Skip to the next interface */
		}

		/* Construct interface name */
		snprintf(iface_name, IFNAMSIZ, "%s%d", INTERFACE_PREFIX, i);

		struct ifreq ifr;
		strncpy(ifr.ifr_name, iface_name, IFNAMSIZ-1);     /* Copy interface name to ifreq */

		/* Use ioctl to get the hardware address */
		if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1)
		{
			close(fd);
			continue;                          /* Skip to the next interface */
		}

		unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
		snprintf(mac_address, sizeof(mac_address), "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
				mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

		close(fd);                             /* Close the socket */

		/* If MAC address is obtained successfully, copy it to buffer and exit loop */
		snprintf(mac_buffer, buffer_size, "%s", mac_address);
		found = 1;
		break;
	}

	if (!found)
	{
		fprintf(stderr, "No active interface found with prefix %s\n", INTERFACE_PREFIX);
		return -1;  /* Return an error if no active interface was found */
	}

	return 0; /* Return success */
}


int main()
{
	int                 fifo_fd;                    /* Descriptor for FIFO file */
	int                 sock;                       /* Descriptor for the socket */
	struct sockaddr_in  saddr;                      /* Structure to store server address */
	char                buffer[BUFFER_SIZE];        /* Buffer to hold data */
	ssize_t             brd;                        /* Number of bytes read */
	/* char server_ip[INET_ADDRSTRLEN]; */  /* Buffer to hold the server IP address */

	/* Get the client IP address at runtime */
	char ip_address[INET_ADDRSTRLEN];       /* Buffer to store the IP address in xxx.xxx.xxx.xxx format */  
	if (get_ip_address(ip_address, sizeof(ip_address)) == 0)
	{
		/* Вывод IP-адреса на консоль */
		printf("IP Address: %s\n", ip_address);
	}

	/* Get the client MAC address at runtime */
	char mac_address[18];                   /* Buffer to store the MAC address in XX:XX:XX:XX:XX:XX format */
	if (get_mac_address(mac_address, sizeof(mac_address)) == 0)
	{
		/* Print MAC address to console */
		printf("MAC Address: %s\n", mac_address);
	}

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

	/* Convert the server's IP address from text to binary form and store it in saddr.sin_addr */
	if (inet_pton(AF_INET, SERVER_IP, &saddr.sin_addr) <= 0)
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

			/* Create a new buffer to hold MAC address and the data */
			char combined_buffer[BUFFER_SIZE + 20]; /* Additional space for MAC address */
			snprintf(combined_buffer, sizeof(combined_buffer), "%s: %s", mac_address, buffer);

			/* Send data to the server */
			if (send(sock, combined_buffer, strlen(combined_buffer), 0) < 0)
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

