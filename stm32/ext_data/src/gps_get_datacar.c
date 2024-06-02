#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "../include/config.h"
#include "../include/file_operations.h"

int main()
{
	char source_dir[MAX_PATH_LENGTH] = "";   // Source directory (read from config)
	char temp_dir[MAX_PATH_LENGTH] = "";     // Temporary directory (read from config)
	char gps_data_dir[MAX_PATH_LENGTH] = ""; // gps_data directory (read from config)

	if (read_config(source_dir, gps_data_dir, temp_dir) != 0)
	{
		return EXIT_FAILURE;
	}

	// Check and create the temporary directory
	struct stat st;
	if (stat(temp_dir, &st) == -1)
	{
		if (mkdir(temp_dir, 0700) != 0)
		{
			perror("Error creating temporary directory");
			return EXIT_FAILURE;
		}
	}

	// Check and create the gps_data directory
	struct stat st2;
	if (stat(gps_data_dir, &st2) == -1)
	{
		if (mkdir(gps_data_dir, 0700) != 0)
		{
			perror("Error creating gps directory");
			return EXIT_FAILURE;
		}
	}

	DIR *dir = opendir(source_dir);
	if (dir == NULL)
	{
		perror("Error opening source directory");
		return EXIT_FAILURE;
	}

	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL)
	{
		// Skip "." (current directory) and ".." (parent directory)
		if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
		{
			if (copy_file(source_dir, temp_dir, ent->d_name) == 0)
			{
				printf("File %s copied successfully.\n", ent->d_name);
			}
			else
			{
				fprintf(stderr, "Failed to copy file %s.\n", ent->d_name);
			}

			if (copy_file(source_dir, gps_data_dir, ent->d_name) == 0)
			{
				printf("File %s copied successfully.\n", ent->d_name);
			}
			else
			{
				fprintf(stderr, "Failed to copy file %s.\n", ent->d_name);
			}
		}
	}

	closedir(dir);
	return EXIT_SUCCESS;
}
