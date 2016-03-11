/**
 * Copyright by Andrea Cervesato <sawk.ita@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 **/

char** strsplit(char* str, const char delim, int* num_of_token);
void strfree(char*** str_array_ptr, size_t length);
