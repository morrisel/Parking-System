/* gcc create_db_empty.c -o out_create_db_empty */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	const char *db_path = "prksys_db.db";

	// Check if the database file already exists
	if (access(db_path, F_OK) != -1)
	{
		// If the file exists, attempt to delete it
		if (remove(db_path) != 0) {
			fprintf(stderr, "Error deleting existing database file.\n");
			return 1;   // Exit with an error code
		}
	}   


	// Construct the SQLite command to create an empty database
	char command[256];
	snprintf(command, sizeof(command), "sqlite %s \"\"", db_path);

	// Execute the SQLite command
	int result = system(command);
	if (result != 0) {
		fprintf(stderr, "Error executing SQLite command.\n");
		return 1;  // Exit with an error code
	}   

	printf("Database created at: %s\n", db_path);

	return 0;
}

