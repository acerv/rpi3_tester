/**
 * Copyright by Andrea Cervesato <sawk.ita@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 **/

#include <stdio.h>
#include <stdlib.h> // calloc
#include <string.h>
#include <pthread.h>
#include <time.h> // nanosleep
#include "gpio.h"

int m_stop = 0;
pthread_t m_blink_thread;

void *blink_led(void* args)
{
    struct timespec tim;
    long time_ns = 0;
    int port = 0;
    int *params;
    int up_down_counter = 0;
    
    if (args != NULL) {
        params = (int*)args;
        port = params[0];
        time_ns = params[1];
        tim.tv_sec = 0;
        tim.tv_nsec = time_ns;

        // this check has been removed from the while,
        // to speed up port writing and do hardware speed tests
        if (time_ns > 0) {
            while (!m_stop) {
                rpi_gpio_fast_up(port);
                if (nanosleep(&tim, NULL) < 0) {
                    printf("Nano sleep system call failed\n");
                    break;
                }
                rpi_gpio_fast_down(port);
                if (nanosleep(&tim, NULL) < 0) {
                    printf("Nano sleep system call failed\n");
                    break;
                }
            }
        } else {
            while (!m_stop) {
                rpi_gpio_fast_up(port);
                rpi_gpio_fast_down(port);
            }
        }
    } else {
        printf("ERROR: Arguments passed to pthread are empty!\n");
    }

    //printf("Thread completed\n");
}

void execute_cmd(char* cmd) 
{
    char* token = NULL;
    int value = 0;
    int port = 0;
    int time_ns = 0;
    int* thread_args;

    // read the command
    token = strtok(cmd, " ");
    if (token != NULL) {
        if (strcmp(token, "read") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                port = atoi(token);
                value = rpi_gpio_read_status(port);
                printf("Pin%d: %d\n", port, value);
            }
            return;
        } else if (strcmp(token, "write") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                port = atoi(token);
                token = strtok(NULL, " ");
                if (token != NULL) {
                    value = atoi(token);
                    rpi_gpio_write_status(port, value);
                    printf("Pin%d: written %d\n", port, value);
                }
            }
            return;
        } else if (strcmp(token, "input") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                port = atoi(token);
                rpi_gpio_set_input(port);
                printf("Pin%d set as input\n", port);
            }
            return;
        } else if (strcmp(token, "output") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                port = atoi(token);
                rpi_gpio_set_output(port);
                printf("Pin%d set as output\n", port);
            }
            return;
        } else if (strcmp(token, "blink") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                if (strcmp(token, "start") == 0) {
                    if (m_blink_thread != 0) {
                        printf ("Blinking already running\n");
                        return;
                    }
                    token = strtok(NULL, " ");
                    if (token != NULL) { // port
                        port = atoi(token);
                        token = strtok(NULL, " ");
                        if (token != NULL) { // time ms
                            time_ns = atoi(token);
                            m_stop = 0;

                            // create the thread and launch it
                            thread_args = (int*)calloc(2, sizeof(int));
                            thread_args[0] = port;
                            thread_args[1] = time_ns;

                            if (pthread_create(&m_blink_thread, NULL, blink_led, (void*)thread_args)) {
                                printf("Error creating blink led thread!\n");
                            } else {
                                printf("Pin%d: blinking started with %dns\n", port, time_ns);
                            }
                        }
                    }
                } else if (strcmp(token, "stop") == 0) {
                    if (m_blink_thread) {
                        m_stop = 1;

                        if (pthread_join(m_blink_thread, NULL)) {
                            printf("Error joining the blink thread!\n");
                        } else {
                            printf("Blinking stopped\n");
                        }

                        m_blink_thread = 0;
                    }
                }
                return;
            }
        }
    }
}

int main()
{
    int input = 0;
    char line[1024];
    char* token = NULL;
    int return_error = 0;

	if ((return_error = rpi_gpio_setup()) != GPIO_NO_ERROR) {
		return 1;
	}
    printf("\nUse one of the following commands:\n"
           "- output \"pin\"\n"
           "- input \"pin\"\n"
           "- read \"pin\"\n"
           "- write \"pin\" \"value\"\n"
           "- blink start \"pin\" \"time ns\"\n"
           "- blink stop\n"
           "- exit\n\n");

    while(1) {
        printf("cmd> ");

        // read the line
        fflush(NULL);
        if (!fgets(line, 1024, stdin)) {
            return 0;
        }

        // remove the new line characters
        token = strtok(line, "\n");
        if (token) {
            if (strcmp(token, "exit") == 0) {
                break; // exit from console mode
            }

            execute_cmd(token);
        }
    }

	if ((return_error = rpi_gpio_cleanup()) != GPIO_NO_ERROR) {
		return 1;
	}

    return 0;
}
