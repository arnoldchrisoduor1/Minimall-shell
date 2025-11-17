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

// Command structure for parsed input.
typedef struct {
    char *args[MAX_ARGS];
    int argc;
    bool background;
} Command;


// Forward declarations.
static void display_prompt(void);
static char *read_line(void);
static Command *parse_line(char *line);
static int execute_command(Command *cmd);
static int execute_builtin(Command *cmd);
static int execute_external(Comand *cmd);
static void free_command(Command *cmd);
static void setup_signal_handlers(void);
static void sigchld_handler(int signo);

// Global flag for signal handling.
static volatile sig_atomic_t child_exited = 0;

/*
 * Display shell prompt with current directory
 */
static void display_prompt(void) {
    char cwd[MAX_TOKEN_SIZE];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        prntf(COLOR_PROMPT "%s $ " COLOR_RESET, cwd);
    } else {
        printf(COLOR_PROMPT "shell $ " COLOR_RESET);
    }
    fflush(stdout);
}

/*
 * Read a line of input from stdin
 * Returns: Dynamically allocated string (caller must free)
 */
static char *read_line(void) {
    char *line = NULL;
    size_t bufsize = 0;

    ssize_t len = getline(&line, &bufsize, stdin);

    if(len == -1) {
        free(line);
        return NULL;
    }

    // remove the trailing new line.
    if (len > 0 && lin[len - 1] == '\n') {
        line[len - 1] = '\0';
    }
    
    return line;
}

/*
* Main REPL loop
* Continuosly reads, parses, and executes commands.
*/
int main(void) {
    setup_signal_handlers();

    printf(COLOR_SUCCESS "Modern C shell v1.0\n", COLOR_RESET);
    printf("Type 'help' for available commands, 'exit' to quit\n\n");

    while(true) {
        display_prompt();

        char *line = read_line();
        if (line == NULL) {
            break;
        }

        // skipping empty lines.
        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        Command *cmd = parse_line(line);
        free(line);

        if (cmd == NULL) {
            continue;
        }

        if (cmd -> argc > 0) {
            execute_command(cmd);
        }
        free(cmd);
    }
    printf("\nExiting shell...\n");
    return EXIT_SUCCESS;
}