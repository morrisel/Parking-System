/**
 * update_prices.c: Program to update prices in a SQLite database from a file
 *
 * This program reads price data from a specified file and updates
 * a SQLite database with the new prices. It also ensures that any
 * entries not present in the file are removed from the database.
 *
 * Compilation:
 *      gcc update_prices.c -o out_update_prices
 *
 * Usage:
 *      ./out_update_prices
 *
 * Features:
 * - Reads from a file defined by DATA_PRICE_FILE.
 * - Updates a SQLite database defined by DB_PATH.
 * - Adds new entries, updates existing ones, and removes missing ones.
 *
 * Version: v1.0
 * Date:    10-06-2024
 * Author:  Morris
 *
 * Date:            Name:               Version:        Modification:
 *   10-06-2024       Morris              v1.0            created
 * 
 * 
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100                           /* Maximum length for a line in the input file */
#define DATA_PRICE_FILE "prices.txt"                  /* Path to the input file containing prices */
#define DB_PATH "prksys_db.db"                       /* Path to the SQLite database */


/* Structure to hold city name and price */
typedef struct
{
    char city[MAX_LINE_LENGTH];   /* city */
    double price;                 /* price */
} CityPrice;


/**
 * load_file_data - Loads data from a file into memory.
 *
 * This function reads city price data from a specified file and stores it in
 * memory. Each line of the file should contain a city name and a price separated
 * by a comma. The function dynamically allocates memory for the data and updates
 * the provided pointers to reflect the loaded data.
 *
 * @param data_price_file: Path to the input file containing prices.
 * @param file_data: Pointer to an array of CityPrice structures.
 * @param file_count: Pointer to a variable to store the number of entries.
 *
 * @return: 0 on success, 1 on failure.
 *
 * Note: The caller is responsible for freeing the allocated memory for file_data.
 */
int load_file_data(const char *data_price_file, CityPrice **file_data, size_t *file_count)
{
    FILE   *file = fopen(data_price_file, "r");
    if (!file)
	{
        fprintf(stderr, "Error opening price file.\n");
        return 1;
    }

    char   line[MAX_LINE_LENGTH];
    size_t count    = 0;
    size_t capacity = 10;
    *file_data = malloc(capacity * sizeof(CityPrice)); /* Allocate initial memory for file data */

    while (fgets(line, sizeof(line), file))
	{
        if (count >= capacity)
		{
            capacity  *= 2;
            *file_data = realloc(*file_data, capacity * sizeof(CityPrice));   /* Expand memory if capacity reached */
        }

        CityPrice *entry = &(*file_data)[count];
        if (sscanf(line, "%99[^,], %lf", entry->city, &entry->price) == 2)
		{
			/* Trim trailing spaces from the city name */
            size_t len = strnlen(entry->city, sizeof(entry->city));
            entry->city[len] = '\0';
            count++;
        }
    }

    fclose(file);
    *file_count = count;

    return 0;
}


/**
 * load_db_data - Loads data from a SQLite database into memory.
 *
 * This function connects to a specified SQLite database and retrieves city price
 * data from the Prices table. The data is stored in memory, and the function
 * dynamically allocates memory for the data and updates the provided pointers
 * to reflect the loaded data.
 *
 * @param db_path: Path to the SQLite database.
 * @param db_data: Pointer to an array of CityPrice structures.
 * @param db_count: Pointer to a variable to store the number of entries.
 *
 * @return: 0 on success, 1 on failure.
 *
 * Note: The caller is responsible for freeing the allocated memory for db_data.
 */
int load_db_data(const char *db_path, CityPrice **db_data, size_t *db_count)
{
    char select_command[256];
    snprintf(select_command, sizeof(select_command),
             "sqlite %s \"SELECT location, price FROM Prices;\"",
             db_path);

    FILE *pipe = popen(select_command, "r");
    if (!pipe)
	{
        fprintf(stderr, "Error executing SQLite command: %s\n", select_command);
        return 1;
    }

    size_t count = 0;
    size_t capacity = 10;
    *db_data = malloc(capacity * sizeof(CityPrice));  /* Allocate initial memory for database data */

    char city[MAX_LINE_LENGTH];
    double price;
    while (fscanf(pipe, "%99[^|]|%lf\n", city, &price) == 2)
	{
        if (count >= capacity)
		{
            capacity *= 2;
            *db_data = realloc(*db_data, capacity * sizeof(CityPrice));  /* Expand memory if capacity reached */
        }

        CityPrice *entry = &(*db_data)[count];
        strncpy(entry->city, city, sizeof(entry->city));
        entry->city[sizeof(entry->city) - 1] = '\0';  /* Ensure null-termination of city name */
        entry->price = price;
        count++;
    }

    pclose(pipe);
    *db_count = count;

    return 0;
}


/**
 * update_prices - Updates prices in the database based on file data.
 *
 * This function compares the prices from the file data with those in the database.
 * If a price in the file data is different from the corresponding price in the
 * database, the database is updated with the new price. If a city in the file data
 * does not exist in the database, it is added.
 *
 * @param db_path: Path to the SQLite database.
 * @param file_data: Array of CityPrice structures containing file data.
 * @param file_count: Number of entries in the file data array.
 *
 * Note: This function assumes that the Prices table exists in the database.
 */
void update_prices(const char *db_path, CityPrice *file_data, size_t file_count)
{
    for (size_t i = 0; i < file_count; i++)
	{
        CityPrice *entry = &file_data[i];
        char select_command[256];
        snprintf(select_command, sizeof(select_command),
                 "sqlite %s \"SELECT price FROM Prices WHERE location='%s';\"",
                 db_path, entry->city);

        FILE *pipe = popen(select_command, "r");
        if (!pipe)
		{
            fprintf(stderr, "Error executing SQLite command: %s\n", select_command);
            continue;
        }

        double existing_price;
        int found = fscanf(pipe, "%lf", &existing_price);  /* Check if city price already exists in database */
        pclose(pipe);

        if (found == 1)
		{
            if (existing_price != entry->price)
			{
                char update_command[256];
                snprintf(update_command, sizeof(update_command),
                         "sqlite %s \"UPDATE Prices SET price=%.2f WHERE location='%s';\"",
                         db_path, entry->price, entry->city);

                int result = system(update_command);  /* Execute update command in SQLite */
                if (result != 0)
				{
                    fprintf(stderr, "Error executing SQLite command: %s\n", update_command);
                }
            }
        }
		else
		{
            char insert_command[256];
            snprintf(insert_command, sizeof(insert_command),
                     "sqlite %s \"INSERT INTO Prices(location, price) VALUES('%s', %.2f);\"",
                     db_path, entry->city, entry->price);

            int result = system(insert_command);  /* Execute insert command in SQLite */
            if (result != 0)
			{
                fprintf(stderr, "Error executing SQLite command: %s\n", insert_command);
            }
        }
    }
}


/**
 * delete_missing_cities - Deletes cities from the database not present in file data.
 *
 * This function identifies cities present in the database but not in the file data,
 * and deletes those cities from the database.
 *
 * @param db_path: Path to the SQLite database.
 * @param db_data: Array of CityPrice structures containing database data.
 * @param db_count: Number of entries in the database data array.
 * @param file_data: Array of CityPrice structures containing file data.
 * @param file_count: Number of entries in the file data array.
 *
 * Note: This function assumes that the database and file data are both sorted and
 *       contain unique city names. It iterates through the database data and checks
 *       each city against the file data, removing those not found in the file.
 */
void delete_missing_cities(const char *db_path, CityPrice *db_data, size_t db_count, CityPrice *file_data, size_t file_count)
{
    for (size_t i = 0; i < db_count; i++)
	{
        CityPrice *db_entry = &db_data[i];
        int found = 0;

        for (size_t j = 0; j < file_count; j++)
		{
            if (strcmp(db_entry->city, file_data[j].city) == 0)  /* Check if city exists in file data */
			{
                found = 1;
                break;
            }
        }

        if (!found)
		{
            char delete_command[256];
            snprintf(delete_command, sizeof(delete_command),
                     "sqlite %s \"DELETE FROM Prices WHERE location='%s';\"",
                     db_path, db_entry->city);

            int result = system(delete_command);  /* Execute delete command in SQLite */
            if (result != 0)
			{
                fprintf(stderr, "Error executing SQLite command: %s\n", delete_command);
            }
        }
    }
}


/**
 * main - Main function to coordinate the update of prices.
 *
 * This function coordinates the overall process of updating prices in the database
 * based on the data from the file. It loads the file data and the database data,
 * updates the prices, deletes missing cities, and frees the allocated memory.
 *
 * @return: 0 on success, 1 on failure.
 */
int main()
{
    const char *db_path = "prksys_db.db";				 /* Path to the SQLite database */
    const char *data_price_file = "prices.txt";          /* Path to the input file containing prices */

    CityPrice *file_data = NULL;                         /* Pointer to hold database data */
    size_t file_count = 0;                               /* Number of entries in file data */
    if (load_file_data(data_price_file, &file_data, &file_count) != 0)
	{
        fprintf(stderr, "Error loading data from file.\n");
        return 1;
    }

    CityPrice *db_data = NULL;                           /* Pointer to hold database data */
    size_t db_count = 0;                                 /* Number of entries in database data */
    if (load_db_data(db_path, &db_data, &db_count) != 0)
	{
        fprintf(stderr, "Erro loading data from database.\n");
        free(file_data);
        return 1;
    }

	/* Update prices in the database based on file data */
    update_prices(db_path, file_data, file_count);

	/* Delete cities from the database that are not present in the file data */
    delete_missing_cities(db_path, db_data, db_count, file_data, file_count);

	/* Free allocated memory */
    free(file_data);
    free(db_data);

    printf("Prices successfully updated.\n");
    return 0;
}

