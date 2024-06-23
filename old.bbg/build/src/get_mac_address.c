#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void get_client_mac(char *mac_buffer, size_t buffer_size)
{
    FILE *fp;
    char cmd[] = "ifconfig | grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}'";

    fp = popen(cmd, "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    if (fgets(mac_buffer, buffer_size, fp) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    // Удаление завершающего символа новой строки
    size_t len = strlen(mac_buffer);
    if (len > 0 && mac_buffer[len-1] == '\n') {
        puts("MAC-ADDRESS:");
        mac_buffer[len-1] = '\0';
    }
    
    pclose(fp);
}

int main()
{
    char server_mac[BUFFER_SIZE];  /* Буфер для хранения MAC-адреса сервера */
    
    // Получение MAC-адреса сервера во время выполнения
    get_client_mac(server_mac, sizeof(server_mac));
    
    // Вывод MAC-адреса на консоль
    printf("Server MAC Address: %s\n", server_mac);
    
    return 0;
}
