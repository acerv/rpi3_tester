/**
 * Copyright by Andrea Cervesato <sawk.ita@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "gpio.h"
#include "strutils.h"

/********************************
 * BLINK COMMAND implementation
 ********************************/
int m_stop_blinking = 1;
pthread_t m_blink_thread = 0;

void* blink_led(void* args)
{
    struct timespec tim;
    long time_ns = 0;
    int port = 0;
    int *params;
    int up_down_counter = 0;

    if (args == NULL) {
        printf("ERROR: Arguments passed to pthread are empty!\n");
        goto ret_statement;
    }

    params = (int*)args;
    port = params[0];
    time_ns = params[1];
    tim.tv_sec = 0;
    tim.tv_nsec = time_ns;

    // set port direction to gitial output
    rpi_gpio_set_output(port);

    // this check has been removed from the while,
    // to speed up port writing and do hardware speed tests
    if (time_ns > 0 && time_ns < 1e10) { 
        while (m_stop_blinking) {
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
        while (!m_stop_blinking) {
            rpi_gpio_fast_up(port);
            rpi_gpio_fast_down(port);
        }
    }

ret_statement:
    //printf("Thread completed\n");
    return NULL;
}

/************************************
 * Frequency reading implementation
 ************************************/
pthread_t m_freq_read_thread = 0;
pthread_t m_clock_read_thread = 0;
int m_stop_freq_read = 1;
unsigned long m_freq_counter = 0;
unsigned long m_current_frequency = 0;

void* clock_read(void* args)
{
    clock_t start_time = 0;
    float time_s = 0;

    start_time = clock();
    while (m_stop_freq_read) {
        time_s = (float)((clock() - start_time)/CLOCKS_PER_SEC);
        if (time_s >= 1) {
            m_current_frequency = m_freq_counter;
            m_freq_counter = 0;
            start_time = clock();
        }
    }
}

void* frequency_read(void* args)
{
    int port = 0;
    int last_status = 0;
    int curr_status = 0;

    if (args == NULL) { // nothing to do
        printf("ERROR: Argument passed to frequency read thread is empty!\n");
        goto ret_statement;
    }

    port = ((int*)args)[0];
    m_freq_counter = 0;

    // select the port direction to input
    rpi_gpio_set_input(port);

    // start reading the square wave
    last_status = rpi_gpio_fast_read(port);
    while (m_stop_freq_read) {
        curr_status = rpi_gpio_fast_read(port);
        if (last_status != curr_status) {
            m_freq_counter++;
        }
        last_status = curr_status;
    }

ret_statement:
    return NULL;
}

/****************
 * The commands
 ****************/
struct command {
    char* label; // the command label
    char* usage; // the usage label
    void (*cmd)(int,char**); // the command function
};

void read_cmd(int num_of_token, char** token_array) {
    int port = 0;
    int value = 0;

    if (num_of_token >= 2 && strcmp(token_array[0], "read") == 0) {
        port = atoi(token_array[1]);
        value = rpi_gpio_read_status(port);
        printf("Pin%d: %d\n", port, value);
    }
}

void write_cmd(int num_of_token, char** token_array) {
    int port = 0;
    int value = 0;

    if (num_of_token >= 3 && strcmp(token_array[0], "write") == 0) {
        port = atoi(token_array[1]);
        value = atoi(token_array[2]);
        rpi_gpio_write_status(port, value);
        printf("Pin%d: written %d\n", port, value);
    }
}

void input_cmd(int num_of_token, char** token_array) {
    int port = 0;

    if (num_of_token >= 2 && strcmp(token_array[0], "input") == 0) {
        port = atoi(token_array[1]);
        rpi_gpio_set_input(port);
        printf("Pin%d set as input\n", port);
    }
}

void output_cmd(int num_of_token, char** token_array) {
    int port = 0;

    if (num_of_token >= 2 && strcmp(token_array[0], "output") == 0) {
        port = atoi(token_array[1]);
        rpi_gpio_set_output(port);
        printf("Pin%d set as output\n", port);
    }
}

void blink_cmd(int num_of_token, char** token_array) {
    int port = 0;
    int time_ns = 0;
    int* thread_args = NULL;

     if (num_of_token >= 2 && strcmp(token_array[0], "blink") == 0) {
        if (num_of_token >= 4 && strcmp(token_array[1], "start") == 0) {
            if (m_blink_thread != 0) {
                printf ("Blinking already running\n");
                goto cleanup;
            }
            port = atoi(token_array[2]);
            time_ns = atoi(token_array[3]);
            m_stop_blinking = 1;

            // create the thread and launch it
            thread_args = (int*)calloc(2, sizeof(int));
            thread_args[0] = port;
            thread_args[1] = time_ns;

            if (pthread_create(&m_blink_thread, NULL, blink_led, (void*)thread_args)) {
               printf("Error creating blink led thread!\n");
               goto cleanup;
            } else {
               printf("Pin%d: blinking started with %dns\n", port, time_ns);
            }
        } else if (strcmp(token_array[1], "stop") == 0) {
            if (m_blink_thread == 0) {
                printf("You need to call \"blink start\" before stop\n");
                goto cleanup;
            }

            m_stop_blinking = 0;

            if (pthread_join(m_blink_thread, NULL)) {
                printf("Error joining the blink thread!\n");
                goto cleanup;
            } else {
                printf("Blinking stopped\n");
            }

            m_blink_thread = 0;
        } else {
            printf("Use \"blink [start|stop] [time_ns]\"\n");
        }
    }

cleanup:
    free(thread_args);
    return;
}

void freq_cmd(int num_of_token, char** token_array) {
    int port = 0;
    int* thread_args = NULL;

    if (num_of_token >= 2 && strcmp(token_array[0], "freq") == 0) {
        if (num_of_token >= 3 && strcmp(token_array[1], "start") == 0) {
            if (m_freq_read_thread != 0 || m_clock_read_thread != 0) {
                printf("Frequency read already running\n");
                goto cleanup;
            }

            port = atoi(token_array[2]);
            m_stop_freq_read = 1;
            thread_args = (int*)malloc(sizeof(int));
            thread_args[0] = port;

            if (pthread_create(&m_freq_read_thread, NULL, frequency_read, thread_args)) {
                printf("Error creating frequency read thread!\n");
                goto cleanup;
            }

            if (pthread_create(&m_clock_read_thread, NULL, clock_read, NULL)) {
                printf("Error creating clock read thread!\n");
                goto cleanup;
            } else {
                printf("Pin%d: frequency read started\n", port);
            }
        } else if (strcmp(token_array[1], "stop") == 0) {
            if (m_freq_read_thread == 0 && m_clock_read_thread == 0) {
                printf("You need to call \"freq start\" before stop\n");
                goto cleanup;
            }

            m_stop_freq_read = 0;

            if (pthread_join(m_freq_read_thread, NULL)) {
                printf("Error joining the frequency read thread\n");
                goto cleanup;
            }

            if (pthread_join(m_clock_read_thread, NULL)) {
                printf("Error joining the clock read thread\n");
                goto cleanup;
            } else {
                printf("Frequency read stopped\n");
            }

            m_freq_read_thread = 0;
            m_clock_read_thread = 0;
        } else if (strcmp(token_array[1], "print") == 0) {
            if (m_freq_read_thread == 0 && m_clock_read_thread == 0) {
                printf("You need to call \"freq start\" before print\n");
                goto cleanup;
            }

            printf("Frequency is %luHz\n", m_current_frequency);
        } else {
            printf("Use \"freq [start|print|stop]\"\n");
        }
    }

cleanup:
    free(thread_args);
    return;
}

static const int m_num_of_commands = 9;
static const struct command m_commands_list[] = {
    {"input",   "input  [gpio]",      input_cmd},
    {"output",  "output [gpio]",      output_cmd},
    {"read",    "read   [gpio]",      read_cmd},
    {"write",   "write  [gpio]",      write_cmd},

    {"blink",   "blink start [gpio] [time_ns]",   blink_cmd},
    {"blink",   "blink stop  [gpio]",             blink_cmd},

    {"freq",    "freq start [pin]",   freq_cmd},
    {"freq",    "freq stop",          freq_cmd},
    {"freq",    "freq print",         freq_cmd}
};

void execute_cmd(char* cmd)
{
    char** token_array = NULL;
    int num_of_token = 0;
    int i = 0;

    token_array = strsplit(cmd, ' ', &num_of_token);
    if (num_of_token <= 0) { // empty command
        goto cleanup;
    }

    for (i = 0; i < m_num_of_commands; i++) {
        if (strcmp(m_commands_list[i].label, token_array[0]) == 0) {
            m_commands_list[i].cmd(num_of_token, token_array);
            break;
        }
    }

cleanup:
    strfree(&token_array, num_of_token);
    return;
}

int main()
{
    int input = 0;
    char line[1024];
    char* token = NULL;
    int return_error = 0;
    int i = 0;

	if ((return_error = rpi_gpio_setup()) != GPIO_NO_ERROR) {
		return 1;
	}

    printf("\nUse one of the following commands:\n");
    for (i = 0; i < m_num_of_commands; i++) {
        printf("- %s\n", m_commands_list[i].usage);
    }
    printf("- exit\n\n");

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
