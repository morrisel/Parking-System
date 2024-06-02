/* gcc create_emp_prkdb.c -o out_create_emp_prkdb */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

void Exit_With_Error_Internal(int status, const char *message)
{
	fprintf(stderr, "ERROR: %s\n", message);
	exit(status);
}

char* get_char(const char *default_name)
{
	static char buffer[256];

	printf("Enter the file name (or press Enter to use \"%s\"): ", default_name);
	if (fgets(buffer, sizeof(buffer), stdin) == NULL)
	{
		Exit_With_Error_Internal(1, "Error reading input.");
	}

	// Remove the newline character at the end of the string, if present
	buffer[strcspn(buffer, "\n")] = '\0';
	if (buffer[0] == '\0') 
	{
		// If the user entered nothing, use the default name
		return (char *)default_name;
	}

	// fflush(stdin);
	// fflush(stdout);
	return buffer;
}

int main(void)
{
	const char *default_db_name = "mydatabase.db";

	// Get the file name from the user
	char *file_name = get_char(default_db_name);

	// Check for root privileges
	if (geteuid() != 0)
	{
		// If not root, attempt to invoke itself through sudo
		char cmd[256];
		snprintf(cmd, sizeof(cmd), "sudo bash -c 'sqlite %s \"\"'", file_name);

		int result = system(cmd);
		if (result != 0)
		{
			Exit_With_Error_Internal(13, "Failed to execute command with root privileges.");
		}


		// Return file ownership to the user who launched the program
		snprintf(cmd, sizeof(cmd), "sudo chown $USER:$USER %s", file_name);
		system(cmd);

		// Set file permissions
		snprintf(cmd, sizeof(cmd), "sudo chmod 664 %s", file_name);
		system(cmd);

		return result; // Exit after calling sudo
	}

	// Main program logic

	const char *db_path = file_name;

	// Check if the database file exists
	if (access(db_path, F_OK) != -1)
	{
		// If the file exists, delete it
		if (remove(db_path) != 0)
		{
			Exit_With_Error_Internal(1, "Error deleting existing database file.");
		}
	}

	// Build the SQLite command to create an empty database
	char command[256];
	snprintf(command, sizeof(command), "sqlite %s \"\"", db_path);

	// Execute the SQLite command
	int result = system(command);
	if (result != 0)
	{
		Exit_With_Error_Internal(1, "Error executing SQLite command.");
	}

	printf("Database created at path: %s\n", db_path);

	return 0;
}
