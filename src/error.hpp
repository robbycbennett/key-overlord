#pragma once


#include <stdio.h>


#define PRINT_ERROR_INNER(message) "keyoverlord: " message "\n"


// Print a message static string to stderr with a new line
#define PRINT_ERROR(message) fwrite(PRINT_ERROR_INNER(message), 1, sizeof(PRINT_ERROR_INNER(message)) - 1, stderr);
