#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>


#define SERVER_PORT            12345                                 /* Server port number */
#define BUFFER_SIZE            1024                                  /* Buffer size for reading and writing data */
#define SHM_KEY                0x1234                                /* Key for shared memory */
#define MAX_CLIENTS            10                                    /* Maximum number of concurrent clients */
#define VERSION                "1.1"                                 /* Server version */


/* Shared memory structure */
struct shared_data
{
    char data[BUFFER_SIZE];                                          /* Buffer to hold shared data */
};


/* Thread argument structure */
struct thread_arg
{
    int                csck;                                         /* Client socket descriptor */
    struct sockaddr_in caddr;                                        /* Client address structure */
};


/* Mutex for shared memory synchronization */
pthread_mutex_t shm_mutex = PTHREAD_MUTEX_INITIALIZER;


/* Signal handler for graceful shutdown */
void signal_handler(int signum);


/**
 * handle_client - Thread function to handle communication with a client.
 *
 * This function reads data from the client, prints it to the terminal,
 * and writes it to shared memory. It uses a mutex to synchronize access
 * to shared memory and signals a semaphore when the client has finished.
 *
 * @arg: Pointer to a thread_arg structure containing the client socket
 *       descriptor and client address.
 *
 * Return: NULL.
 */
void *handle_client(void *arg);



#endif    /* SERVER_H */
