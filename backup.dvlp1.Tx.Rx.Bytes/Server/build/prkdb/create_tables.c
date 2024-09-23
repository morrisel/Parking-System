/* gcc create_tables.c -o out_create_tables */
#include <stdio.h>
#include <stdlib.h>

int main()
{

    system("sqlite prksys_db.db \
        \"CREATE TABLE Prices \
            (id INTEGER PRIMARY KEY, \
             location TEXT NOT NULL, \
             price REAL NOT NULL);\"");

    system("sqlite prksys_db.db \
        \"CREATE TABLE Customer_Data \
            (id INTEGER PRIMARY KEY, \
             mac_address TEXT NOT NULL, \
             status CHAR(1) NOT NULL, \
             x REAL NOT NULL, \
             y REAL NOT NULL, \
             z REAL NOT NULL);\"");

    printf("Tables created successfully in the database.\n");

    return 0;
}

