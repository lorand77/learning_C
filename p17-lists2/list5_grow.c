// The list now grows instead of filling up: when it runs out of room,
// realloc to cap * 1.25. Starts at 16. Never shrinks.

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INITIAL_CAP 16

int *numbers;
int len = 0;
int cap = 0;

// pos must be in 0..max
void check_index(const char *who, int pos, int max) {
  if (pos < 0 || pos > max) {
    fprintf(stderr, "%s: index %d out of bounds (len=%d)\n", who, pos, len);
    exit(1);
  }
}

// replaces the old "list is full" error: make room instead of refusing.
void make_room(void) {
  if (len < cap) {
    return;
  }

  if (cap > INT_MAX / 2) {   // the addition below would overflow
    fprintf(stderr, "make_room: list too large\n");
    exit(1);
  }

  // integer math: cap + cap/4 is cap * 1.25 without floats.
  // 16 -> 20 -> 25 -> 31 -> 38 -> 47 ...
  int newcap = cap + cap / 4;
  if (newcap <= cap) {       // tiny caps round down to no growth at all
    newcap = cap + 1;
  }
  printf("grow: %d -> %d\n", cap, newcap);

  // into a temporary first: if realloc fails it returns NULL and the
  // old block is still valid, so assigning straight to numbers leaks it.
  int *bigger = realloc(numbers, (size_t)newcap * sizeof(int));
  if (bigger == NULL) {
    fprintf(stderr, "realloc failed\n");
    exit(1);
  }
  numbers = bigger;
  cap = newcap;
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
  make_room();
  numbers[len] = value;
  len++;
}

void insert(int pos, int value) {
  make_room();
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

int main() {
  numbers = malloc(INITIAL_CAP * sizeof(int));
  if (numbers == NULL) {
    fprintf(stderr, "malloc failed\n");
    exit(1);
  }
  cap = INITIAL_CAP;

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

  // push past 16 to watch it grow
  for (int i = 0; i < 100; i++) {
    append(i);
  }
  printf("len=%d cap=%d\n", len, cap);

  free(numbers);
  return 0;
}
