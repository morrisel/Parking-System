/* gcc shared_mem_sync.c -o out_shared_mem_sync */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>

#define SHARED_MEM_KEY 1234
#define MAX_COORDINATE_SIZE 20

typedef struct {
	int x;
	int y;
	int z;
} Coordinates;

void sync_shared_mem_to_db(const char *db_path)
{
	int shmid = shmget(SHARED_MEM_KEY, sizeof(Coordinates), 0666 | IPC_CREAT);
	if (shmid == -1) {
		perror("Ошибка создания разделяемой памяти");
		exit(1);
	}

	Coordinates *shared_coords = (Coordinates*) shmat(shmid, NULL, 0);
	if (shared_coords == (void*) -1) {
		perror("Ошибка подключения к разделяемой памяти");
		exit(1);
	}

	char command[100]; // Буфер для команды SQLite
	snprintf(command, sizeof(command), "sqlite %s", db_path);

	// Формируем SQL-запрос для вставки координат в таблицу Customer_Data
	snprintf(command + strlen(command), sizeof(command) - strlen(command),
			" \"INSERT INTO Customer_Data (x, y, z) VALUES (%d, %d, %d);\"",
			shared_coords->x, shared_coords->y, shared_coords->z);

	// Выполняем команду SQLite для вставки данных из разделяемой памяти в базу данных
	int result = system(command);
	if (result != 0) {
		fprintf(stderr, "Ошибка выполнения команды SQLite.\n");
	}

	// Отключаемся от разделяемой памяти
	shmdt(shared_coords);
}

int main() {
	const char *db_path = "mydatabase.db";

	sync_shared_mem_to_db(db_path);

	printf("Данные из разделяемой памяти успешно синхронизированы с базой данных.\n");
	return 0;
}

