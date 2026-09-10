CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion

BINS = sum.bin sum-memsize.bin array-memsize.bin

all: $(BINS)

sum.bin: sum.c
	$(CC) $(CFLAGS) -o sum.bin sum.c

sum-memsize.bin: sum-memsize.c
	$(CC) $(CFLAGS) -o sum-memsize.bin sum-memsize.c

array-memsize.bin: array-memsize.c
	$(CC) $(CFLAGS) -o array-memsize.bin array-memsize.c

clean:
	rm -f $(BINS)
