#ifndef UPDATE_PRICES_H
#define UPDATE_PRICES_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH        100                                   /* Maximum length for a line in the input file */
#define DATA_PRICE_FILE        "prices.txt"                          /* Path to the input file containing prices */
#define DB_PATH                "prksys_db.db"                        /* Path to the SQLite database */


/* Structure to hold city name and price */
typedef struct
{   
    char city[MAX_LINE_LENGTH];                                      /* city */
    double price;                                                    /* price */
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
int load_file_data(const char *data_price_file, CityPrice **file_data, size_t *file_count);


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
int load_db_data(const char *db_path, CityPrice **db_data, size_t *db_count);


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
void update_prices(const char *db_path, CityPrice *file_data, size_t file_count);


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
void delete_missing_cities(const char  *db_path,           CityPrice   *db_data, 
                           size_t      db_count,           CityPrice   *file_data,
                           size_t      file_count);










#endif  /* UPDATE_PRICES_H */ 
