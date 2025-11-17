/*
 * Modern POSIX Shell Implementation
 * Demonstrates process management, parsing, and system programming concepts
 * C17 compliant with clean architecture
 */

 #define _POSIX_C_SOURCE 200809L
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <sys/wait.h>
 #include <errno.h>
 #include <stdbool.h>
 #include <signal.h>

//  Configuration constants.
#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64
#define MAX_TOKEN_SIZE 256

// color codes for enhanced UX
#define COLOR_RESET     "\033[0m"
#define COLOR_PROMPT    "\033[1;36m"
#define COLOR_ERROR   "\033[1;31m"
#define COLOR_SUCCESS "\033[1;32m"