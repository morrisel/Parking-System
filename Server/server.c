/** 
 * server.c: Multithreaded TCP server with shared memory and semaphore synchronization
 *
 * This program sets up a multithreaded TCP server that listens for incoming
 * connections, receives data from clients, and writes it to shared memory. The
 * program uses mutexes and semaphores for synchronization and supports
 * graceful shutdown using signal handling.
 *
 * Compilation:
 *      gcc server.c -o out_server -lpthread
 *
 * Usage:
 *      ./out_server
 *
 * Features:
 * - Listens for incoming connections on a port defined by SERVER_PORT.
 * - Uses shared memory to store received data.
 * - Supports multiple concurrent clients using threads.
 * - Synchronizes access to shared memory using mutexes.
 * - Limits the number of concurrent clients using semaphores.
 * - Handles signals for graceful shutdown.
 *
 * Version: v1.0
 * Date:    24-03-2024
 * Author:  Morris
 *                       
 * Date:            Name:               Version:        Modification:
 *   24-03-2024       Morris              v1.0            created
 *   03-06-2024       Moriis              v1.1            updated
 *                                                          - Added multithreading support
 *                                                          - Added mutexes and semaphores for synchronization
 *                                                          - Updated parallelism management
 *                                                          - Added signal handling for graceful shutdown
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <signal.h>

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

/* Semaphore for controlling the number of concurrent clients */
sem_t client_sem;

/* Flag to control the main loop */
volatile sig_atomic_t running = 1;

/* Signal handler for graceful shutdown */
void signal_handler(int signum)
{
    running = 0;                                                     /* Set flag to stop the main loop */
    printf("Received signal %d, shutting down...\n", signum);
}


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
void *handle_client(void *arg)
{
    struct thread_arg   *targ   = (struct thread_arg *)arg;
    int                 csck    = targ->csck;
    struct sockaddr_in  caddr   = targ->caddr;
    char                buffer[BUFFER_SIZE + 1];
    int                 tbrecv;                                      /* Total bytes received from client */
    int                 brecv;
    char                *line;                                       /* Split the buffer into lines */

    /* Attach shared memory */
    int shm_id = shmget(SHM_KEY, sizeof(struct shared_data), 0666);
    if (shm_id == -1)
    {
        perror("shmget");
        close(csck);
        free(arg);
        pthread_exit(NULL);
    }
    struct shared_data *shm_data = (struct shared_data *)shmat(shm_id, NULL, 0);
    if (shm_data == (void *)-1)
    {
        perror("shmat");
        close(csck);
        free(arg);
        pthread_exit(NULL);
    }

    /* Read data from the client and print it to the terminal */
    while ((brecv = recv(csck, buffer, BUFFER_SIZE, 0)) > 0)
    {
        tbrecv         =  0;
        buffer[brecv]  =  '\0';                                      /* Null-terminate the string */
        line           =  strtok(buffer, "\n");                      /* Tokenize within the recv loop */

        while (line != NULL)
        {
            printf("Received from %s: %s\n", inet_ntoa(caddr.sin_addr), line);  /* Print each line */

            /* Lock mutex */
            pthread_mutex_lock(&shm_mutex);

            /* Write to shared memory */
            strncpy(shm_data->data, line, BUFFER_SIZE);

            /* Unlock mutex */
            pthread_mutex_unlock(&shm_mutex);

            line = strtok(NULL, "\n");                               /* Move to the next line */
        }

        tbrecv += brecv;
    }

    /* Print a message indicating the end of data reception from the client */
    if (tbrecv > 0)
    {
        printf("Complete message received from client.\n");
    }

    /* Detach shared memory */
    shmdt(shm_data);

    /* Close the client socket */
    close(csck);
    free(arg);

    /* Signal the semaphore to indicate a client has finished */
    sem_post(&client_sem);

    pthread_exit(NULL);
}

int main()
{
    int                 ssck;
    struct sockaddr_in  saddr;
    pthread_t           thread_id;

    /* Print version information */
    printf("Server Version: %s\n", VERSION);

    /* Set up signal handler for SIGINT (Ctrl+C) */
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        perror("Error setting signal handler");
        exit(EXIT_FAILURE);
    }

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

    /* Initialize shared memory */
    int shm_id = shmget(SHM_KEY, sizeof(struct shared_data), IPC_CREAT | 0666);
    if (shm_id == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    /* Initialize the semaphore */
    if (sem_init(&client_sem, 0, MAX_CLIENTS) == -1)
    {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    /* Main loop to accept and handle client connections */
    while (running)
    {
        /* Allocate memory for thread argument */
        struct thread_arg *targ = malloc(sizeof(struct thread_arg));
        if (!targ)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        /* Wait for a semaphore signal (only allow MAX_CLIENTS concurrent clients) */
        sem_wait(&client_sem);

        /* Accept a connection from a client */
        socklen_t caddrlen  =  sizeof(targ->caddr);
        targ->csck          =  accept(ssck, (struct sockaddr *)&targ->caddr, &caddrlen);
        if (targ->csck < 0)
        {
            perror("accept");
            free(targ);
            /* Signal the semaphore since this client failed to connect */
            sem_post(&client_sem);
            continue;
        }

        /* Create a thread to handle the client */
        if (pthread_create(&thread_id, NULL, handle_client, (void *)targ) != 0)
        {
            perror("pthread_create");
            close(targ->csck);
            free(targ);
            /* Signal the semaphore since this client failed to create a thread */
            sem_post(&client_sem);
        }

        /* Detach the thread */
        pthread_detach(thread_id);
    }

    /* Cleanup: close the server socket */
    close(ssck);
    /* Cleanup: destroy the semaphore */
    sem_destroy(&client_sem);

    return 0;
}

