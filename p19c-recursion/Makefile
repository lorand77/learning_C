CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion

# every .c in this directory is a standalone program, built twice:
# foo.bin without optimisation (gcc's default -O0), foo_O2.bin with -O2.
SRCS := $(wildcard *.c)
BINS := $(SRCS:.c=.bin)
BINS_O2 := $(SRCS:.c=_O2.bin)

all: $(BINS) $(BINS_O2)

%.bin: %.c
	$(CC) $(CFLAGS) -o $@ $<

%_O2.bin: %.c
	$(CC) $(CFLAGS) -O2 -o $@ $<

clean:
	rm -f $(BINS) $(BINS_O2)

.PHONY: all clean
