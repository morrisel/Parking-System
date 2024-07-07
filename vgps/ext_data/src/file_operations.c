/** -------------------------------------------------------- file_operations.c --- *
 *                                                                                 *
 *  - Contains functions for file operations, such as copying files.               *
 *                                                                                 *
 * ------------------------------------------------------------------------------ **/
#include <stdio.h>
#include <stdlib.h>
#include "../include/file_operations.h"  /* Include custom file operations header file */
#include "../include/config.h"           /* Include custom configuration header file */


/**
 * Copies a file from the source directory to the destination directory.
 */
int copy_file(const char *source_dir, const char *dest_dir, const char *filename)
{
    char source_path[MAX_PATH_LENGTH];  /* Buffer to store the source file path */
    char dest_path[MAX_PATH_LENGTH];    /* Buffer to store the destination file path */
    
    /* Construct the full source file path */
    snprintf(source_path, MAX_PATH_LENGTH, "%s/%s", source_dir, filename);

    /* Construct the full destination file path */
    snprintf(dest_path, MAX_PATH_LENGTH, "%s/%s", dest_dir, filename);
    
    FILE *source = fopen(source_path, "rb");  /* Open the source file for reading */
    if (source == NULL)
    {
        perror("Error opening source file");
        return -1;
    }
    
    FILE *dest = fopen(dest_path, "wb");  /* Open or create the destination file for writing */
    if (dest == NULL)
    {
        perror("Error opening destination file");
        fclose(source);
        return -1;
    }
    
    char buffer[BUFFER_SIZE];  /* Buffer to store data read from the source file */
    size_t bytes_read;  /* Number of bytes read from the source file */
    
    /* Copy data from the source file to the destination file */
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, source)) > 0)
    {
        if (fwrite(buffer, 1, bytes_read, dest) != bytes_read)
	{
            perror("Error writing to destination file");
            fclose(source);
            fclose(dest);
            return -1;
        }
    }

    fclose(source);
    fclose(dest);
    
    return 0;
}
