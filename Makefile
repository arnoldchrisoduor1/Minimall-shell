Minishell Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = minishell

SRCS = main.c sh_input.c sh_parser.c sh_execute.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c shell.h
$(CC) $(CFLAGS) -c $< -o $@

clean:
rm -f $(OBJS) $(TARGET)

.PHONY: all clean