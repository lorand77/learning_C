CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion

BINS = sum.bin 

all: $(BINS)

sum.bin: sum.c
	$(CC) $(CFLAGS) -o sum.bin sum.c

# generate assembly for study:  make sum.s
%.s: %.c
	$(CC) $(CFLAGS) -S -o $@ $<

clean:
	rm -f $(BINS) $(BINS:.bin=.s)
