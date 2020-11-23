NAME = zoption
OS = $(shell uname | sed 's/[_ ].*//')
LDFLAGS =
CLANGFLAGS = -g -O0 -Wall -Werror -std=c99 -Wno-unused -Wno-format-security -fsanitize=address -fsanitize-undefined-trap-on-error
GCCFLAGS = -g -Wall -Werror -std=c99 -Wno-unused -DLT_TABDUMP
CFLAGS = $(CLANGFLAGS)
CFLAGS = $(GCCFLAGS)
CC = clang
CC = gcc
PREFIX = /usr/local
VERSION = 0.01

test:
	$(CC) $(CFLAGS) -o $(NAME)-test $(NAME).c main.c

