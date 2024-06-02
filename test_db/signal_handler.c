/* gcc signal_handler.c -o out_signal_handler */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define UPDATE_SIGNAL SIGUSR1

void signal_handler(int signum) {
	if (signum == UPDATE_SIGNAL) {
		printf("Сигнал получен для обновления цен.\n");
		// Здесь вы можете добавить код для обновления цен
	}
}

int main() {
	// Устанавливаем обработчик сигнала для обновления цен
	if (signal(UPDATE_SIGNAL, signal_handler) == SIG_ERR) {
		fprintf(stderr, "Ошибка установки обработчика сигнала.\n");
		return 1;
	}

	printf("Ожидание сигнала для обновления цен...\n");
	while (1) {
		sleep(1); // Поддерживаем программу в работе
	}

	return 0;
}

