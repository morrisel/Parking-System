#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void get_client_ip(char *ip_buffer, size_t buffer_size)
{
    FILE *fp;
    char cmd[] = "hostname -I | awk '{print $1}'";

    fp = popen(cmd, "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    if (fgets(ip_buffer, buffer_size, fp) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    // Удаление завершающего символа новой строки
    size_t len = strlen(ip_buffer);
    if (len > 0 && ip_buffer[len-1] == '\n')
    {
        puts("hello");
        ip_buffer[len-1] = '\0';
    }
    
    pclose(fp);
}

int main()
{
    char server_ip[INET_ADDRSTRLEN];  /* Буфер для хранения IP-адреса сервера */
    
    // Получение IP-адреса сервера во время выполнения
    get_client_ip(server_ip, sizeof(server_ip));
    
    // Вывод IP-адреса на консоль
    printf("Server IP: %s\n", server_ip);
    
    return 0;
}

