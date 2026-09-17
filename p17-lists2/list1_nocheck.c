// Fixed array list, no bounds checking: a bad index quietly reads or
// writes whatever sits next in memory.

#include <stdio.h>

#define CAP 16

int numbers[CAP];
int len = 0;

int get(int pos) {
  return numbers[pos];
}

void set(int pos, int value) {
  numbers[pos] = value;
}

void append(int value) {
  numbers[len] = value;
  len++;
}

void insert(int pos, int value) {
  for (int i = len; i > pos; i--) {
    numbers[i] = numbers[i - 1];
  }
  numbers[pos] = value;
  len++;
}

void delete_at(int pos) {
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
