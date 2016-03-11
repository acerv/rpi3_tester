#ifndef RPI_GPIO_H
#define RPI_GPIO_H 

/**
 * Copyright by Andrea Cervesato <sawk.ita@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 **/

#define GPIO_STATUS_HIGH 1
#define GPIO_STATUS_LOW  0

#define GPIO_NO_ERROR 0
#define GPIO_SETUP_FAILED -1
#define GPIO_CLEANUP_FAILED = -2

int rpi_gpio_setup(void);
int rpi_gpio_cleanup(void);

void rpi_gpio_set_input(int port);
void rpi_gpio_set_output(int port);

void rpi_gpio_write_status(int port, int status);
int rpi_gpio_read_status(int port);

void rpi_gpio_fast_up(int port);
void rpi_gpio_fast_down(int port);
int rpi_gpio_fast_read(int port);

#endif
