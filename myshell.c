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
 * Parse input line into Command structure
 * Handles tokenization and argument splitting
 */
static Command *parse_line(char *line) {
    Command *cmd = calloc(1, sizeof(Command));
    if (cmd == NULL) {
        perror("calloc");
        return NULL;
    }

    cmd->background = false;
    cmd->argc = 0;

    char *token;
    char *saveptr;

    // Tokenize by whitespace.
    token = strtok_r(line, "\t\r\n", &saveptr);

    while (token != NULL && cmd->argc < MAX_ARGS - 1) {
        if(strcmp(token, "&") == 0) {
            cmd->background = true;
            break;
        }

        // Allcate and copy token.
        cmd->args[cmd->argc] = strdup(token);
        if (cmd->args[cmd->argc] == NULL) {
            perror("strdup");
            free_command(cmd);
            return NULL;
        }
        cmd->argc++;
        token = strtok_r(NULL, " \t\r\n", &saveptr);
    }
    // NULL-terminate argv array (required by execvp)
    cmd->args[cmd->argc] = NULL;

    return cmd;
}

/*
 * Execute external command using fork/exec pattern
 * Demonstrates core process management concepts
 */
static int execute_external(Command *cmd) {
    pid_t pid = fork();
    
    if (pid < 0) {
        // Fork failed
        perror("fork");
        return 1;
    }
    
    if (pid == 0) {
        // Child process: execute the command
        execvp(cmd->args[0], cmd->args);
        
        // execvp only returns on error
        fprintf(stderr, COLOR_ERROR "%s: command not found\n" COLOR_RESET, 
                cmd->args[0]);
        exit(EXIT_FAILURE);
    }
    
    // Parent process
    if (!cmd->background) {
        // Foreground: wait for child to complete
        int status;
        pid_t wait_result;
        
        do {
            wait_result = waitpid(pid, &status, 0);
        } while (wait_result == -1 && errno == EINTR);
        
        if (wait_result == -1) {
            perror("waitpid");
            return 1;
        }
        
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    } else {
        // Background: don't wait
        printf("[Background] Process %d started\n", pid);
    }
    
    return 0;
}


/*
 * Execute a command - dispatches to builtin or external
 */
static int execute_command(Command *cmd) {
    if (cmd->argc == 0) {
        return 0;
    }
    
    // Check if it's a builtin command
    int result = execute_builtin(cmd);
    
    if (result == -1) {
        // Not a builtin, execute as external command
        return execute_external(cmd);
    }
    
    return result;
}

/*
 * Execute builtin commands (must run in shell process)
 */
static int execute_builtin(Command *cmd) {
    const char *command = cmd->args[0];
    
    // cd - change directory
    if (strcmp(command, "cd") == 0) {
        const char *path = (cmd->argc > 1) ? cmd->args[1] : getenv("HOME");
        
        if (path == NULL) {
            fprintf(stderr, COLOR_ERROR "cd: HOME not set\n" COLOR_RESET);
            return 1;
        }
        
        if (chdir(path) != 0) {
            perror("cd");
            return 1;
        }
        return 0;
    }
    
    // exit - terminate shell
    if (strcmp(command, "exit") == 0) {
        int exit_code = (cmd->argc > 1) ? atoi(cmd->args[1]) : 0;
        exit(exit_code);
    }
    
    // help - display help information
    if (strcmp(command, "help") == 0) {
        printf("\nModern C Shell - Available Commands:\n");
        printf("  cd [dir]     - Change directory\n");
        printf("  exit [code]  - Exit shell\n");
        printf("  help         - Display this help\n");
        printf("  pwd          - Print working directory\n");
        printf("  <command> &  - Run command in background\n");
        printf("\nAny other command will be executed as an external program.\n\n");
        return 0;
    }
    
    // pwd - print working directory
    if (strcmp(command, "pwd") == 0) {
        char cwd[MAX_TOKEN_SIZE];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
            return 0;
        } else {
            perror("pwd");
            return 1;
        }
    }
    
    return -1;  // Not a builtin
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