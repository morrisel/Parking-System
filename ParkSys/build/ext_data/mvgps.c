/* mvgps.c - Spawns a child process to run 'out_gps_creator',
   then runs 'gps_get_datacar' if the child succeeds. This program
   demonstrates basic process management using fork() and execl().

   Compilation: gcc mvgps.c -o out_mvgps */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
    pid_t pid;
    int status;

    /* Fork a child process */
    pid = fork();

    if (pid < 0)
    {
		/* Error handling: fork failed */
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) /* Child process */
    {
		/* Change working directory to "vgps" */
        if (chdir("vgps") != 0)
        {
            perror("chdir failed");
            exit(EXIT_FAILURE);
        }

		/* Execute "out_gps_creator" */
        execl("./out_gps_creator", "out_gps_creator", (char *)NULL);

		/* If execl returns, there was an error */
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    else
	{	/* Parent process */

        /* Wait for child process to finish */
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid failed");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status))
        {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
            if (WEXITSTATUS(status) == 0)
            {
				/* Execute "gps_get_datacar" only if child exited successfully */
                execl("./gps_get_datacar", "gps_get_datacar", (char *)NULL);
				
				/* If execl returns, there was an error */
                perror("execl failed");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            printf("Child process did not exit successfully\n");
        }
    }

    return 0;
}
