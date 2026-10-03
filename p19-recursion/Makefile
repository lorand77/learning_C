CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion
# big integers come from GMP
LDLIBS = -lgmp

# every .c in this directory is a standalone program.
SRCS := $(wildcard *.c)
BINS := $(SRCS:.c=.bin)

all: $(BINS)

%.bin: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

clean:
	rm -f $(BINS)

.PHONY: all clean
