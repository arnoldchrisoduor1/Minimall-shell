#include "shell.h"

void sh_loop(void) 
{
    char *line;
    char **args;
    int status = 1;

    while (status) {
        // 1 . READING: display prompt and read input line.
        printf("<minishell> ");
        line = sh_read_line();

        // check if sh_read_line and read returned NULL (EOF or error)
        if (line == NULL) {
            status = 0;
            continue;
        }

        // PARSE
        // TODO: to be impemented in sh_parser.c
        args = sh_split_line(line);

        // 3 . EXECUTE  - only executed if arguements were successfully parsed.
        if (args != NULL) {
            status = sh_execute(args);
        } else {
            // just continue if args is NULL.
        }

        free(line);

        if (args != NULL) {
            free(args);
        }
    }
}