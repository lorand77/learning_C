CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion

# every .c in this directory is a standalone program.
SRCS := $(wildcard *.c)
BINS := $(SRCS:.c=.bin)

all: $(BINS)

%.bin: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BINS)

.PHONY: all clean
