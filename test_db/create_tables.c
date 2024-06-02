/* gcc create_tables.c -o out_create_tables */
#include <stdio.h>
#include <stdlib.h>

int main()
{
	// Создаем таблицу Prices
	system	("sqlite mydatabase.db 					   \
					\"CREATE TABLE Prices (            \
					id INTEGER PRIMARY KEY,            \
					location TEXT NOT NULL,            \
					price REAL NOT NULL ); \"          \
			");

	// Создаем таблицу Customer_Data
	system("sqlite mydatabase.db \
	       			\"CREATE TABLE Customer_Data (     \
		            id INTEGER PRIMARY KEY,            \
					x INTEGER NOT NULL,                \
					y INTEGER NOT NULL,                \
					z INTEGER NOT NULL);\"             \
			");

//	// Заполняем таблицу Prices тестовыми данными
//	system("sqlite mydatabase.db \"INSERT INTO Prices (location, price) VALUES ('Zone A', 5.00);\"");
//	system("sqlite mydatabase.db \"INSERT INTO Prices (location, price) VALUES ('Zone B', 3.50);\"");
//	system("sqlite mydatabase.db \"INSERT INTO Prices (location, price) VALUES ('Zone C', 2.00);\"");
//
//	// Пример предварительного заполнения таблицы Customer_Data тестовыми данными
//	system("sqlite mydatabase.db \"INSERT INTO Customer_Data (x, y, z) VALUES (10, 20, 30);\"");
//	system("sqlite mydatabase.db \"INSERT INTO Customer_Data (x, y, z) VALUES (15, 25, 35);\"");
//	system("sqlite mydatabase.db \"INSERT INTO Customer_Data (x, y, z) VALUES (20, 30, 40);\"");

	printf("Таблицы созданы и заполнены данными в базе данных.\n");
	return 0;
}

