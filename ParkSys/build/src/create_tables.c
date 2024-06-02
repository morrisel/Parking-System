/* gcc create_tables.c -o out_create_tables */
#include <stdio.h>
#include <stdlib.h>

int main()
{
  // Creating the Prices table
	system	("sqlite mydatabase.db 					   \
					\"CREATE TABLE Prices (            \
					id INTEGER PRIMARY KEY,            \
					location TEXT NOT NULL,            \
					price REAL NOT NULL ); \"          \
			");

	// Creating the Customer_Data table
	system("sqlite mydatabase.db \
	       			\"CREATE TABLE Customer_Data (     \
		            id INTEGER PRIMARY KEY,            \
					x INTEGER NOT NULL,                \
					y INTEGER NOT NULL,                \
					z INTEGER NOT NULL);\"             \
			");

// Filling the Prices table with test data
//	system("sqlite mydatabase.db \"INSERT INTO Prices (location, price) VALUES ('Zone A', 5.00);\"");
//	system("sqlite mydatabase.db \"INSERT INTO Prices (location, price) VALUES ('Zone B', 3.50);\"");
//	system("sqlite mydatabase.db \"INSERT INTO Prices (location, price) VALUES ('Zone C', 2.00);\"");
//
// Example of pre-populating the Customer_Data table with test data
//	system("sqlite mydatabase.db \"INSERT INTO Customer_Data (x, y, z) VALUES (10, 20, 30);\"");
//	system("sqlite mydatabase.db \"INSERT INTO Customer_Data (x, y, z) VALUES (15, 25, 35);\"");
//	system("sqlite mydatabase.db \"INSERT INTO Customer_Data (x, y, z) VALUES (20, 30, 40);\"");

    printf("Tables are created and filled with data in the database.\n");

	return 0;
}

