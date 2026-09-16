#include <limits.h>
#include <stdio.h>

#define CAP 16

int numbers[CAP];
int len = 0;

// C has no exceptions, so a bad index has to show up in the return value.
// set/append/insert/delete_at return 1 if they did the work, 0 if they refused.
// get is the odd one out -- see below.

// returns the element itself, so a bad index comes back as INT_MIN --
// the smallest int, picked because a real list is unlikely to hold it.
// still not airtight: INT_MIN is a legal element value like any other.
int get(int pos) {
  if (pos < 0 || pos >= len) {
    return INT_MIN;
  }
  return numbers[pos];
}

int set(int pos, int value) {
  if (pos < 0 || pos >= len) {
    return 0;
  }
  numbers[pos] = value;
  return 1;
}

int append(int value) {
  if (len >= CAP) {
    return 0;
  }
  numbers[len] = value;
  len++;
  return 1;
}

// pos == len is allowed here: it means "insert at the end"
int insert(int pos, int value) {
  if (len >= CAP || pos < 0 || pos > len) {
    return 0;
  }
  for (int i = len; i > pos; i--) {
    numbers[i] = numbers[i - 1];
  }
  numbers[pos] = value;
  len++;
  return 1;
}

int delete_at(int pos) {
  if (pos < 0 || pos >= len) {
    return 0;
  }
  for (int i = pos; i < len - 1; i++) {
    numbers[i] = numbers[i + 1];
  }
  len--;
  return 1;
}

int main() {
  append(10);
  append(20);
  append(30);

  int first = get(0);

  set(1, 25);

  int count = len;

  append(40);

  insert(0, 15);
  //insert(-1000, 15);
  //insert(10, 15);

  delete_at(1);

  for (int i = 0; i < len; i++) {
    printf("%d ", numbers[i]);
  }
  printf("\n");

  return 0;
}
