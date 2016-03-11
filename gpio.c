/**
 * Copyright by Andrea Cervesato <sawk.ita@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 **/

#include "gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>

// device address
#define BCM2708_BASE_DEFAULT	0x20000000
#define BCM2709_BASE_DEFAULT	0x3f000000
#define GPIO_BASE_OFFSET	    0x200000

// operations registers
#define OFFSET_FSEL 0
#define OFFSET_SET  7
#define OFFSET_CLR  10
#define OFFSET_READ 13

// pages dimension
#define PAGE_SIZE		(4*1024)
#define BLOCK_SIZE		(4*1024)

// number of ports
#define MAX_GPIO_PINS 7
#define CHECK_PIN(p) \
    do { \
        if (p < 0)\
            p = 0; \
        else if (p >= MAX_GPIO_PINS) \
            p = MAX_GPIO_PINS - 1; \
    } while(0)

// the virtual memory associated with the gpio memory
static volatile uint32_t *gpio_map;

// this array maps the gpio pin with its register on board
static int pinToGpio[7] = 
{
    17, 18, 27, 22, 23, 24, 25 // digital I/O
};

int rpi_gpio_setup(void) 
{
	int mem_fd = 0; // file descriptor
    int gpio_base = 0;
    int gpio_dev = 0;
	uint8_t *gpio_mem = NULL; // virtual memory block

	// try using /dev/mem method
	if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC)) < 0) {
		printf("Gpio setup can't open /dev/mem device.\n");
		return GPIO_SETUP_FAILED;
	}

	if ((gpio_mem = malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL) {
		printf("Gpio setup can't allocate space for /dev/mem device\n");
		return GPIO_SETUP_FAILED;
	}

	if ((uint32_t)gpio_mem % PAGE_SIZE) {
		gpio_mem += PAGE_SIZE - ((uint32_t)gpio_mem % PAGE_SIZE);
	}

    gpio_dev = BCM2709_BASE_DEFAULT;
    gpio_base = gpio_dev + GPIO_BASE_OFFSET;

	gpio_map = (uint32_t*)mmap((void*)gpio_mem, 
		BLOCK_SIZE,
		PROT_READ|PROT_WRITE,
		MAP_SHARED|MAP_FIXED,
		mem_fd,
		gpio_base);

	if ((uint32_t)gpio_map < 0) {
		printf("Gpio setup can't access to virtual GPIO memory.\n");
		return GPIO_SETUP_FAILED;
	}

	return GPIO_NO_ERROR;
}

int rpi_gpio_cleanup(void)
{
	munmap((void*)gpio_map, BLOCK_SIZE);
	return GPIO_NO_ERROR;
}

// direction: 0 is IN, 1 is OUT
static void rpio_gpio_inner_set_direction(int pin, int direction) 
{
    int offset = 0;
    int shift = 0;
    int port = 0;
    
    CHECK_PIN(pin);

    port = pinToGpio[pin];

    // selection bits are organized by cluster of 10 gpio ports
    offset = OFFSET_FSEL + (port / 10);

    // the selection bits are 3 and they are placed one next each other
    shift = (port % 10) * 3;

    if (direction) { // output
        *(gpio_map + offset) = (*(gpio_map + offset) & ~(7 << shift)) | (1 << shift);
    } else { // input
        *(gpio_map + offset) = (*(gpio_map + offset) & ~(7 << shift));
    }
}

void rpi_gpio_set_input(int port) 
{
    rpio_gpio_inner_set_direction(port, 0);
}

void rpi_gpio_set_output(int port)
{
    rpio_gpio_inner_set_direction(port, 1);
}

void rpi_gpio_write_status(int pin, int status) 
{
    int offset = 0;
    int port = 0;

    CHECK_PIN(pin);
    port = pinToGpio[pin];

   if (status) { // HIGH
       offset = OFFSET_SET + (port / 32);
   } else { // LOW
       offset = OFFSET_CLR + (port / 32);
   }
   
   *(gpio_map + offset) = 1 << (port % 32);
}

int rpi_gpio_read_status(int pin)
{
    int offset = 0;
    int port = 0;
    int value = 0;

    CHECK_PIN(pin);
    port = pinToGpio[pin];

    offset = OFFSET_READ + (port / 32);
    value = *(gpio_map + offset) & (1 << (port % 32));

    // move the port bit in the less significant bit position,
    // so the routine results will be 0 or 1
    value = value >> port;

    return value;
}

// This function sets a digital pin to up state.
// Use with caution: no bounds check on pin number line
inline void rpi_gpio_fast_up(int pin) 
{
    int port = pinToGpio[pin];
    *(gpio_map + OFFSET_SET + (port/32)) = 1 << (port%32);
}

// This function sets a digital pin to down state
// Use with caution: no bounds check on pin number line
inline void rpi_gpio_fast_down(int pin)
{
    int port = pinToGpio[pin];
    *(gpio_map + OFFSET_CLR + (port/32)) = 1 << (port%32);
}

// This function returns the digital state of a gpio line.
// If returns value is 0 the status is low, up otherwise.
// Use with caution: no bounds check on pin number line
inline int rpi_gpio_fast_read(int pin)
{
    int port = pinToGpio[pin];
    return *(gpio_map + OFFSET_READ + (port/32)) & (1<< (port%32));
}
