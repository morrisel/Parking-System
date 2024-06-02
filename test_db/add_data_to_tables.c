/* gcc add_data_to_tables.c -o out_add_data_to_tables */
#include <stdio.h>
#include <stdlib.h>

int main()
{
    
	// Заполняем таблицу Prices тестовыми данными
	system("sqlite mydatabase.db \"INSERT INTO Prices (location, price) VALUES ('Zone A', 5.00);\"");
	system("sqlite mydatabase.db \"INSERT INTO Prices (location, price) VALUES ('Zone B', 3.50);\"");
	system("sqlite mydatabase.db \"INSERT INTO Prices (location, price) VALUES ('Zone C', 2.00);\"");


	// Пример предварительного заполнения таблицы Customer_Data тестовыми данными
	system("sqlite mydatabase.db \"INSERT INTO Customer_Data (x, y, z) VALUES (10, 20, 30);\"");
	system("sqlite mydatabase.db \"INSERT INTO Customer_Data (x, y, z) VALUES (15, 25, 35);\"");
	system("sqlite mydatabase.db \"INSERT INTO Customer_Data (x, y, z) VALUES (20, 30, 40);\"");



    printf("Initial data inserted into the database.\n");
    return 0;
}

