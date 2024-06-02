/* gcc update_prices.c -o out_update_prices */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

void update_prices(const char *db_path, const char *prices_file_path)
{
    FILE *prices_file = fopen(prices_file_path, "r");
    if (prices_file == NULL) {
        fprintf(stderr, "Ошибка открытия файла с ценами.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), prices_file)) {
        // Извлекаем город и цену из строки
        char city[MAX_LINE_LENGTH];
        double price;
        if (sscanf(line, "%[^,],%lf", city, &price) != 2) {
            fprintf(stderr, "Ошибка разбора строки цены: %s\n", line);
            continue;
        }

        // Формируем SQL-запрос для обновления цены
        char command[MAX_LINE_LENGTH + 100];
        snprintf(command, sizeof(command),
                 "sqlite %s \"UPDATE Prices SET price=%.2f WHERE location='%s';\"",
                 db_path, price, city);

        // Выполняем команду SQLite для обновления цены
        int result = system(command);
        if (result != 0) {
            fprintf(stderr, "Ошибка выполнения команды SQLite: %s\n", command);
            continue;
        }

//		// Очищаем команду для следующей итерации
//		command[strlen("sqlite") + strlen(db_path)] = '\0';
    }

    fclose(prices_file);
}

int main() {
	const char *db_path = "mydatabase.db";
	const char *prices_file_path = "prices.txt";

	update_prices(db_path, prices_file_path);

	printf("Цены успешно обновлены.\n");
	return 0;
}

