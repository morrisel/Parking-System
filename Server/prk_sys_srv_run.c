/* gcc prk_sys_srv_run.c -o out_prk_sys_srv_run */                   /* Compile command to create the executable */
/* prk_sys_srv_run.c: Program to start multiple services in separate terminal windows */

/*
 * prk_sys_srv_run.c: Program to start multiple services in separate terminal windows
 *
 * This program sequentially starts several specified programs in new terminal windows.
 * Each program is given a brief period to initialize before the next program is started.
 *
 * Programs started:
 * - out_server
 * - out_listener
 * - out_giis
 * - out_insert_data_from_giis_shm
 *
 * The `run_in_terminal` function constructs a command to open a new terminal window
 * and execute a specified program within it, leaving the terminal open after the
 * program finishes execution.
 *
 * The `main` function prints status messages and adds delays between starting each program.
 *
 * Compilation:
 *      gcc prk_sys_srv_run.c -o out_prk_sys_srv_run
 *
 * Usage:
 *      ./out_prk_sys_srv_run
 *
 * Features:
 * - Opens a new terminal window for each program.
 * - Adds delays to ensure each program has time to initialize.
 * - Prints status messages before starting each program.
 * 
 * Version: 1.0
 * Author: Morris
 *
 * Date:                Name:                 Modification:
 *      01-07-2024           Morris                  created
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/**
 * run_in_terminal - Executes a program in a new terminal window.
 *
 * This function constructs a command to open a new terminal window and
 * execute the specified program within it. The terminal remains open
 * after the program finishes execution.
 *
 * @param program: The name of the program to run.
 *
 * Note: This function uses `gnome-terminal` and `bash` to run the command.
 *       If `gnome-terminal` is not available, or if the system command fails,
 *       the function prints an error message and exits the program.
 */
void run_in_terminal(const char *program)
{
    char command[256];
    snprintf(command, sizeof(command), "gnome-terminal -- bash -c './%s; exec bash'", program);
    if (system(command) == -1)
    {
        perror("system");
        exit(EXIT_FAILURE);
    }
}

/**
 * main - Entry point of the program.
 *
 * This function sequentially starts several programs in new terminal windows.
 * It provides delays between each program start to ensure proper initialization.
 *
 * Programs started:
 * - out_server
 * - out_listener
 * - out_giis
 * - out_insert_data_from_giis_shm
 *
 * Return: 0 on successful completion.
 *
 * Note: The function prints messages to the console before starting each program.
 *       Delays are added using `sleep` to ensure each program has time to initialize.
 */
int main()
{
    printf("Starting out_server...\n");
    run_in_terminal("out_server");
    sleep(2);                                                        /* Give time for the server to start */

    printf("Starting out_listener...\n");
    run_in_terminal("out_listener");
    sleep(2);                                                        /* Give time for the listener to start */

    printf("Starting out_giis...\n");
    run_in_terminal("out_giis");
    sleep(2);                                                        /* Give time for giis to start */

    printf("Staring out_insert_data_from_giis_shm...\n");
    run_in_terminal("out_insert_data_from_giis_shm");
    sleep(1);                                                        /* Give time for insert_data_from_giis_shm to start */

    return 0;
}

