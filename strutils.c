/**
 * Copyright by Andrea Cervesato <sawk.ita@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 **/

#include <stdlib.h>
#include <string.h>

// This function splits a string into substrings delimited by a character.
// It's not super fast, but it's fast enough for general purpose.
char** strsplit(char* str, const char delim, size_t* num_of_token)
{
    int i = 0;
    int length = 0;
    char* tmp = NULL;
    char** token_array = NULL;
    char* token = NULL;
    *num_of_token = 0;
   
    if (str == NULL) { // nothing to return
        return NULL;
    }

    if (delim == '\0') { // return a copy of the input string
        token_array = (char**)calloc(1, sizeof(char*));
        token_array[0] = (char*)calloc(strlen(str), sizeof(char));
        strcpy(token_array[0], str);
        *num_of_token = 1;
        return token_array;
    }

    // count how many tokens will be saved
    tmp = str;
    while (*tmp != '\0') {
        if (*tmp == delim) {
            (*num_of_token)++;
        }
        tmp++;
    }

    if (*num_of_token <= 0) {
        return token_array;
    }

    // create the array of substrings
    (*num_of_token)++; // num_of_token = num_of_delimiter + 1
    token_array = (char**)calloc(*num_of_token, sizeof(char*));

    // read tokens
    tmp = (char*)malloc(sizeof(char));
    tmp[0] = delim;
    token = strtok(str, tmp);
    i = 0;

    while (token != NULL) {
        length = strlen(str);
        token_array[i] = (char*)calloc(length, sizeof(char));
        strcpy(token_array[i], token);
        token = strtok(NULL, tmp);
        i++;
    }

    return token_array;
}

void strfree(char*** str_array_ptr, int length) 
{
    int i = 0;
    char** str_array = NULL;
    if (str_array_ptr == NULL || length <= 0) {
        return;
    }

    str_array = *str_array_ptr;
    for (i = 0; i < length; i++) {
        free(str_array[i]);
    }

    free(str_array);
    str_array_ptr = NULL;
}
