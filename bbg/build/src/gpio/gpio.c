/**
 * gpio.c
 *
 * This module provides a basic interface for interacting with GPIO pins
 * on a Linux-based system. It includes functions for exporting/unexporting
 * GPIO pins, setting their direction and value, and reading their value.
 *
 * The GPIO functionality is implemented through interaction with the 
 * sysfs interface, which allows user-space applications to control GPIOs.
 *
 * Version: v1.0
 * Date:    04-09-2024
 * Author:  Morris
 *
 * Date:            Name:            Version:    Modification:
 * 04-09-2024       Morris           v1.0        created
 *
 */

#include "gpio.h"
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

/**
 * gpio_init
 *
 * Initialize a GPIO pin by setting its number and exporting it for use.
 *
 * Parameters:
 *   @gpio: Pointer to a gpio_t structure to initialize.
 *   @number: GPIO pin number to initialize.
 *
 * Return: void
 */
void gpio_init(gpio_t *gpio, int number) 
{
    gpio->number = number;
    snprintf(gpio->gpioPath, sizeof(gpio->gpioPath), "/sys/class/gpio/gpio%d/", number);
    gpio_export(gpio);
}

/**
 * gpio_deinit
 *
 * Deinitialize a GPIO pin by unexporting it.
 *
 * Parameters:
 *   @gpio: Pointer to a gpio_t structure to deinitialize.
 *
 * Return: void
 */
void gpio_deinit(gpio_t *gpio) 
{
    gpio_unexport(gpio);
}

/**
 * gpio_export
 *
 * Export a GPIO pin to make it accessible through the sysfs interface.
 *
 * Parameters:
 *   @gpio: Pointer to a gpio_t structure representing the GPIO to export.
 *
 * Return: void
 *
 * If the export operation fails, an error message is printed.
 */
void gpio_export(gpio_t *gpio) 
{
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd != -1) 
    {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%d", gpio->number);
        write(fd, buffer, strlen(buffer));
        close(fd);
    }
    else 
    {
        perror("Unable to export gpio");
    }
}

/**
 * gpio_unexport
 *
 * Unexport a GPIO pin, removing its sysfs entry.
 *
 * Parameters:
 *   @gpio: Pointer to a gpio_t structure representing the GPIO to unexport.
 *
 * Return: void
 *
 * If the unexport operation fails, an error message is printed.
 */
void gpio_unexport(gpio_t *gpio) 
{
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd != -1) 
    {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%d", gpio->number);
        write(fd, buffer, strlen(buffer));
        close(fd);
    }
    else 
    {
        perror("Unable to unexport gpio");
    }
}

/**
 * gpio_set_direction
 *
 * Set the direction of a GPIO pin (input or output).
 *
 * Parameters:
 *   @gpio: Pointer to a gpio_t structure representing the GPIO.
 *   @direction: A string specifying the direction ("in" or "out").
 *
 * Return: void
 *
 * If setting the direction fails, an error message is printed.
 */
void gpio_set_direction(gpio_t *gpio, const char *direction) 
{
    char path[256];
    snprintf(path, sizeof(path), "%sdirection", gpio->gpioPath);

    int fd = open(path, O_WRONLY);
    if (fd != -1) 
    {
        write(fd, direction, strlen(direction));
        close(fd);
    }
    else 
    {
        perror("Unable to set direction");
    }
}

/**
 * gpio_set_value
 *
 * Set the value of a GPIO pin (high or low).
 *
 * Parameters:
 *   @gpio: Pointer to a gpio_t structure representing the GPIO.
 *   @value: The value to set (1 for high, 0 for low).
 *
 * Return: void
 *
 * If setting the value fails, an error message is printed.
 */
void gpio_set_value(gpio_t *gpio, int value) 
{
    char path[256];
    snprintf(path, sizeof(path), "%svalue", gpio->gpioPath);

    int fd = open(path, O_WRONLY);
    if (fd != -1) 
    {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%d", value);
        write(fd, buffer, strlen(buffer));
        close(fd);
    }
    else 
    {
        perror("Unable to set value");
    }
}

/**
 * gpio_get_value
 *
 * Get the current value of a GPIO pin (high or low).
 *
 * Parameters:
 *   @gpio: Pointer to a gpio_t structure representing the GPIO.
 *
 * Return:
 *   The current value of the GPIO pin (1 for high, 0 for low).
 *   Returns -1 if an error occurs.
 *
 * If reading the value fails, an error message is printed.
 */
int gpio_get_value(gpio_t *gpio) 
{
    char path[256];
    snprintf(path, sizeof(path), "%svalue", gpio->gpioPath);

    int fd = open(path, O_RDONLY);
    if (fd != -1) 
    {
        char buffer[10];
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
        close(fd);
        if (bytesRead > 0) 
        {
            buffer[bytesRead] = '\0';
            return atoi(buffer);
        }
    }
    else 
    {
        perror("Unable to get value");
    }
    return -1;
}

