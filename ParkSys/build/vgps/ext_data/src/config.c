/** ----------------------------------------------------------------- config.c --- *
 *                                                                                 *
 *  - Reads the configuration setting from a 'dirlinks.conf' file and populates    *
 *    the provided directories.                                                    *
 *                                                                                 *
 * ------------------------------------------------------------------------------ **/
#include <stdio.h>
#include <string.h>
#include "../include/config.h"

#define CONFIG_FILE "dirlinks.conf"

/**
 * This function reads the configuration file specified by CONFIG_FILE and
 * assigns values to the provided directory paths based on the configuration
 * file contents.
 */
int read_config(char *src_dir, char *gps_data_dir, char *temp_dir)
{
    FILE *config_file = fopen(CONFIG_FILE, "r");

    if (config_file == NULL)
    {
        perror("Error opening config file");
        return -1;
    }

    char line[MAX_PATH_LENGTH];
    /* Read each line from the configuration file */
    while (fgets(line, MAX_PATH_LENGTH, config_file) != NULL)
    {
        if (strncmp(line, "SOURCE_DIR=", 11) == 0)
        {
            strncpy(src_dir, line + 11, MAX_PATH_LENGTH - 1);
            src_dir[strcspn(src_dir, "\n")] = 0;  /* Remove newline */
        }
        else if (strncmp(line, "GPS_DATA_DIR=", 13) == 0)
        {
            strncpy(gps_data_dir, line + 13, MAX_PATH_LENGTH - 1);
            gps_data_dir[strcspn(gps_data_dir, "\n")] = 0;   /* Remove newline */
        }
        else if (strncmp(line, "TEMP_DIR=", 9) == 0)
        {
            strncpy(temp_dir, line + 9, MAX_PATH_LENGTH - 1);
            temp_dir[strcspn(temp_dir, "\n")] = 0;   /* Remove newline */
        }
    }

    fclose(config_file);
    return 0;
}
