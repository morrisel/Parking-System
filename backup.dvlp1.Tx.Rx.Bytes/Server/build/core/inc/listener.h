#ifndef LISTENER_H
#define LISTENER_H


#define SHM_KEY                0x1234                                /* Key for shared memory segment */
#define BUFFER_SIZE            1024                                  /* Buffer size for data */
#define FIFO_NAME              "giis/ipc_transfer_giis"              /* Path to the FIFO file */
/*#define FIFO_TO_DB           "giis/ipc_to_db" */                   /* (Optional) Path to another FIFO file */


/* Shared memory structure */
struct shared_data
{
    char data[BUFFER_SIZE];                                          /* Data buffer */
};


/**
 * signal_handler - Signal handler to set the running flag to 0.
 *
 * @signum: The signal number that was received.
 */
void signal_handler(int signum);


#endif  /* LISTENER_H */
