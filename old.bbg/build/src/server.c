/* server.c - Версия 1.1
 * Изменения:
 * - Добавлена поддержка многопоточности
 * - Добавлены мьютексы и семафоры для синхронизации
 * - Обновлено управление параллелизмом
 * - Добавлена обработка сигналов для корректного завершения работы
 */

/* gcc server.c -o out_server -lpthread */
/* server.c */

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

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024
#define SHM_KEY 0x1234
#define MAX_CLIENTS 10
#define VERSION "1.1"

// Shared memory structure
struct shared_data {
    char data[BUFFER_SIZE];
};

// Thread argument structure
struct thread_arg {
    int csck;
    struct sockaddr_in caddr;
};

// Mutex for shared memory synchronization
pthread_mutex_t shm_mutex = PTHREAD_MUTEX_INITIALIZER;

// Semaphore for controlling the number of concurrent clients
sem_t client_sem;

// Flag to control the main loop
volatile sig_atomic_t running = 1;

// Signal handler for graceful shutdown
void signal_handler(int signum) {
    running = 0;  // Set flag to stop the main loop
    printf("Received signal %d, shutting down...\n", signum);
}

void *handle_client(void *arg) {
    struct thread_arg *targ = (struct thread_arg *)arg;
    int csck = targ->csck;
    struct sockaddr_in caddr = targ->caddr;
    char buffer[BUFFER_SIZE + 1];

	int                 tbrecv;         /* Total bytes received from client */
    int 				brecv;
    char 				*line;			// Split the buffer into lines

    // Attach shared memory
    int shm_id = shmget(SHM_KEY, sizeof(struct shared_data), 0666);
    if (shm_id == -1) {
        perror("shmget");
        close(csck);
        free(arg);
        pthread_exit(NULL);
    }
    struct shared_data *shm_data = (struct shared_data *)shmat(shm_id, NULL, 0);
    if (shm_data == (void *)-1) {
        perror("shmat");
        close(csck);
        free(arg);
        pthread_exit(NULL);
    }

    // Read data from the client and print it to the terminal
    while ((brecv = recv(csck, buffer, BUFFER_SIZE, 0)) > 0) {

		tbrecv = 0;

        buffer[brecv] = '\0';       /* Null-terminate the string */
        line = strtok(buffer, "\n");       /* Tokenize within the recv loop */
        while (line != NULL) {
            printf("Received from %s: %s\n", inet_ntoa(caddr.sin_addr), line);  /* Print each line */

            // Lock mutex
            pthread_mutex_lock(&shm_mutex);

            // Write to shared memory
            strncpy(shm_data->data, line, BUFFER_SIZE);

            // Unlock mutex
            pthread_mutex_unlock(&shm_mutex);

            line = strtok(NULL, "\n");       /* Move to the next line */
        }

		tbrecv += brecv;
    }

	/* Print a message indicating the end of data reception from the client */
	if (tbrecv > 0)
	{
		printf("Complete message received from client.\n");
	}

    // Detach shared memory
    shmdt(shm_data);

    // Close the client socket
    close(csck);
    free(arg);

    // Signal the semaphore to indicate a client has finished
    sem_post(&client_sem);

    pthread_exit(NULL);
}

int main() {
    int ssck;
    struct sockaddr_in saddr;
    pthread_t thread_id;

    // Print version information
    printf("Server Version: %s\n", VERSION);

    // Set up signal handler for SIGINT (Ctrl+C)
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("Error setting signal handler");
        exit(EXIT_FAILURE);
    }

    // Create a socket
    if ((ssck = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SERVER_PORT);
    saddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the port
    if (bind(ssck, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(ssck, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", SERVER_PORT);

    // Initialize shared memory
    int shm_id = shmget(SHM_KEY, sizeof(struct shared_data), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Initialize the semaphore
    if (sem_init(&client_sem, 0, MAX_CLIENTS) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    while (running) {
        struct thread_arg *targ = malloc(sizeof(struct thread_arg));
        if (!targ) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        // Wait for a semaphore signal (only allow MAX_CLIENTS concurrent clients)
        sem_wait(&client_sem);

        // Accept a connection from a client
        socklen_t caddrlen = sizeof(targ->caddr);
        targ->csck = accept(ssck, (struct sockaddr *)&targ->caddr, &caddrlen);
        if (targ->csck < 0) {
            perror("accept");
            free(targ);
            // Signal the semaphore since this client failed to connect
            sem_post(&client_sem);
            continue;
        }

        // Create a thread to handle the client
        if (pthread_create(&thread_id, NULL, handle_client, (void *)targ) != 0) {
            perror("pthread_create");
            close(targ->csck);
            free(targ);
            // Signal the semaphore since this client failed to create a thread
            sem_post(&client_sem);
        }

        // Detach the thread
        pthread_detach(thread_id);
    }

    // Cleanup
    close(ssck);
    sem_destroy(&client_sem);

    return 0;
}

