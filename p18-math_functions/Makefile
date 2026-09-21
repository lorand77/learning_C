CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion
# math.h needs an explicit link against libm
LDLIBS = -lm

# every .c in this directory is a standalone program.
# skip names containing spaces (e.g. "test2 copy.c") -- make cannot
# express them as targets, and they would break the whole build.
SRCS := $(shell ls *.c 2>/dev/null | grep -v ' ')
BINS := $(SRCS:.c=.bin)

all: $(BINS)

%.bin: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

# generate assembly for study:  make test.s
%.s: %.c
	$(CC) $(CFLAGS) -S -o $@ $<

clean:
	rm -f $(BINS) $(SRCS:.c=.s)

.PHONY: all clean
