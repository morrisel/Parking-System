/* gcc select_from_prices.c -o out_select_from_prices */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Формируем команду для выполнения SQL-запроса SELECT
    char command[256];
    snprintf(command, sizeof(command), "sqlite mydatabase.db \"SELECT * FROM Prices\"");

    // Выполняем команду и получаем вывод
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Ошибка выполнения команды SQLite.\n");
        return 1;
    }

    // Читаем вывод команды и выводим его
    char line[1024];
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }

    // Закрываем поток
    pclose(fp);

    return 0;
}

