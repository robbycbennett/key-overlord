#pragma once


#include <stdio.h>


#define FAIL_INNER(message) "keyoverlord: " message "\n"


// Print a message static string to stderr with a new line and return 1
#define FAIL(message) return fwrite(FAIL_INNER(message), 1, sizeof(FAIL_INNER(message)) - 1, stderr), 1;
