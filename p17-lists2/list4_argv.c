// Same again, but the capacity is a command line argument:
//
//   ./list4_argv.bin 16
//
// This is what malloc buys: a global array needs its size at compile
// time, so CAP stops being a #define and becomes a variable.

#include <stdio.h>
#include <stdlib.h>

int *numbers;   // no memory until main calls malloc
int len = 0;
int cap = 0;    // was CAP, now filled in from argv

// pos must be in 0..max
void check_index(const char *who, int pos, int max) {
  if (pos < 0 || pos > max) {
    fprintf(stderr, "%s: index %d out of bounds (len=%d)\n", who, pos, len);
    exit(1);
  }
}

void check_room(const char *who) {
  if (len >= cap) {
    fprintf(stderr, "%s: list is full (cap=%d)\n", who, cap);
    exit(1);
  }
}

int get(int pos) {
  check_index("get", pos, len - 1);
  return numbers[pos];
}

void set(int pos, int value) {
  check_index("set", pos, len - 1);
  numbers[pos] = value;
}

void append(int value) {
  check_room("append");
  numbers[len] = value;
  len++;
}

void insert(int pos, int value) {
  check_room("insert");
  check_index("insert", pos, len);  // len is allowed: insert at the end
  for (int i = len; i > pos; i--) {
    numbers[i] = numbers[i - 1];
  }
  numbers[pos] = value;
  len++;
}

void delete_at(int pos) {
  check_index("delete_at", pos, len - 1);
  for (int i = pos; i < len - 1; i++) {
    numbers[i] = numbers[i + 1];
  }
  len--;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <capacity>\n", argv[0]);
    exit(1);
  }

  // argv is text, so "16" arrives as characters. strtol reports where it
  // stopped reading, which is how "16x" gets rejected.
  char *rest;
  long n = strtol(argv[1], &rest, 10);
  if (*rest != '\0' || n < 1 || n > 1000000) {
    fprintf(stderr, "capacity must be a number between 1 and 1000000\n");
    exit(1);
  }
  cap = (int)n;

  numbers = malloc((size_t)cap * sizeof(int));  // bytes, not elements
  if (numbers == NULL) {
    fprintf(stderr, "malloc failed\n");
    exit(1);
  }

  append(10);
  append(20);
  append(30);
  set(1, 25);
  append(40);
  insert(0, 15);
  delete_at(1);

  for (int i = 0; i < len; i++) {
    printf("%s%d", i > 0 ? " " : "", get(i));
  }
  printf("\n");

  free(numbers);
  return 0;
}
