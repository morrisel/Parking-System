/* gcc create_db_empty.c -o out_create_db_empty */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    const char *db_path = "mydatabase.db";

    // Проверяем, существует ли файл базы данных
    if (access(db_path, F_OK) != -1) {
        // Если файл существует, удаляем его
        if (remove(db_path) != 0) {
            fprintf(stderr, "Ошибка удаления существующего файла базы данных.\n");
            return 1;
        }
    }   

    // Строим команду SQLite для создания пустой базы данных
    char command[256];
    snprintf(command, sizeof(command), "sqlite %s \"\"", db_path);

    // Выполняем команду SQLite
    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Ошибка выполнения команды SQLite.\n");
        return 1;
    }   

    printf("База данных создана по пути: %s\n", db_path);
    return 0;
}

