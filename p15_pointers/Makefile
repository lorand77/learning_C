CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion

BINS = test.bin layout.bin

all: $(BINS)

test.bin: test.c
	$(CC) $(CFLAGS) -o test.bin test.c

layout.bin: layout.c
	$(CC) $(CFLAGS) -o layout.bin layout.c

# generate assembly for study:  make test.s
%.s: %.c
	$(CC) $(CFLAGS) -S -o $@ $<

clean:
	rm -f $(BINS) $(BINS:.bin=.s)
