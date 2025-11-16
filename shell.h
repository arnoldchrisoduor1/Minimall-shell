#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Created the function prototype for the read module.
char *sh_read_line(void);

// function prototype for the parse module
char **sh_split_line(char *line);

// function prototypes for the execute module.
int sh_execute(char **args);


// Constants.
#define SH_RL_BUFSIZE 1024
#define SH_TOK_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n\a"

#endif
