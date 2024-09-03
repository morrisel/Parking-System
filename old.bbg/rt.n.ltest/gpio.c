#include "gpio.h"
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

/*
 * gpio_init - Initializes the GPIO pin
 * @gpio: The GPIO structure to initialize
 * @number: The GPIO pin number
 *
 * This function sets the GPIO pin number and constructs the path
 * to the corresponding sysfs entry. It then exports the pin so
 * that it can be used by the application.
 */
void gpio_init(gpio_t *gpio, int number)
{
    gpio->number = number;
    snprintf(gpio->gpioPath, sizeof(gpio->gpioPath), "/sys/class/gpio/gpio%d/", number);
    gpio_export(gpio);
}

/*
 * gpio_deinit - Deinitializes the GPIO pin
 * @gpio: The GPIO structure to deinitialize
 *
 * This function unexports the GPIO pin, effectively releasing it.
 */
void gpio_deinit(gpio_t *gpio)
{
    gpio_unexport(gpio);
}

/*
 * gpio_export - Exports the GPIO pin to sysfs
 * @gpio: The GPIO structure representing the pin
 *
 * This function writes the GPIO pin number to the export file,
 * making the pin available for use in user space.
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

/*
 * gpio_unexport - Unexports the GPIO pin from sysfs
 * @gpio: The GPIO structure representing the pin
 *
 * This function writes the GPIO pin number to the unexport file,
 * removing the pin from user space.
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

/*
 * gpio_set_direction - Sets the direction of the GPIO pin
 * @gpio: The GPIO structure representing the pin
 * @direction: The desired direction ("in" or "out")
 *
 * This function writes the specified direction to the direction file,
 * configuring the GPIO pin as either an input or an output.
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

/*
 * gpio_set_value - Sets the value of the GPIO pin
 * @gpio: The GPIO structure representing the pin
 * @value: The value to set (0 or 1)
 *
 * This function writes the specified value to the value file,
 * driving the GPIO pin to the desired state.
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

/*
 * gpio_get_value - Reads the value of the GPIO pin
 * @gpio: The GPIO structure representing the pin
 *
 * This function reads the current value from the value file,
 * returning the state of the GPIO pin (0 or 1).
 *
 * Returns the value of the GPIO pin, or -1 if an error occurred.
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

