// Same list, but every index is checked. C has no exception to raise,
// so a bad index prints to stderr and exits.

#include <stdio.h>
#include <stdlib.h>

#define CAP 16

int numbers[CAP];
int len = 0;

// pos must be in 0..max
void check_index(const char *who, int pos, int max) {
  if (pos < 0 || pos > max) {
    fprintf(stderr, "%s: index %d out of bounds (len=%d)\n", who, pos, len);
    exit(1);
  }
}

void check_room(const char *who) {
  if (len >= CAP) {
    fprintf(stderr, "%s: list is full (cap=%d)\n", who, CAP);
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

int main() {
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

  return 0;
}
