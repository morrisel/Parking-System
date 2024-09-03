#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    char gpioPath[64];
    int number;
} gpio_t;

void gpio_init(gpio_t *gpio, int number);
void gpio_deinit(gpio_t *gpio);

void gpio_export(gpio_t *gpio);
void gpio_unexport(gpio_t *gpio);

void gpio_set_direction(gpio_t *gpio, const char *direction);
void gpio_set_value(gpio_t *gpio, int value);
int gpio_get_value(gpio_t *gpio);

#endif

