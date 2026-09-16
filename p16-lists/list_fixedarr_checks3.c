// Same as list_fixedarr_checks2.c (a bad index is fatal), but the storage
// comes from malloc instead of being a fixed global array.
//
// CAP is unchanged: this still holds 16 elements and still cannot grow.
// The only difference is where the memory lives -- the heap, asked for at
// run time, instead of a block the compiler reserved before main started.

#include <stdio.h>
#include <stdlib.h>

#define CAP 16

// just a pointer now: no memory attached to it until main calls malloc.
// using it before that would read a wild address.
int *numbers;
int len = 0;

// stderr, not stdout, so the message stays separate from the program's output.
// exit(1) means "failed"; exit(0) would mean success.
void die(const char *who, int pos) {
  fprintf(stderr, "%s: index %d is out of bounds (len=%d)\n", who, pos, len);
  exit(1);
}

// nothing returns a status code any more -- if a call comes back at all,
// it worked. so get can just hand back the element, with no INT_MIN sentinel.
int get(int pos) {
  if (pos < 0 || pos >= len) {
    die("get", pos);
  }
  return numbers[pos];
}

void set(int pos, int value) {
  if (pos < 0 || pos >= len) {
    die("set", pos);
  }
  numbers[pos] = value;
}

void append(int value) {
  if (len >= CAP) {
    fprintf(stderr, "append: list is full (CAP=%d)\n", CAP);
    exit(1);
  }
  numbers[len] = value;
  len++;
}

// pos == len is allowed here: it means "insert at the end"
void insert(int pos, int value) {
  if (len >= CAP) {
    fprintf(stderr, "insert: list is full (CAP=%d)\n", CAP);
    exit(1);
  }
  if (pos < 0 || pos > len) {
    die("insert", pos);
  }
  for (int i = len; i > pos; i--) {
    numbers[i] = numbers[i - 1];
  }
  numbers[pos] = value;
  len++;
}

void delete_at(int pos) {
  if (pos < 0 || pos >= len) {
    die("delete_at", pos);
  }
  for (int i = pos; i < len - 1; i++) {
    numbers[i] = numbers[i + 1];
  }
  len--;
}

int main() {
  // ask for room for CAP ints. sizeof(int) matters: malloc counts bytes,
  // not elements, so CAP alone would reserve a quarter of what is needed.
  numbers = malloc(CAP * sizeof(int));

  // malloc returns NULL if it could not get the memory. unlike a global
  // array, this can genuinely fail, so it has to be checked.
  if (numbers == NULL) {
    fprintf(stderr, "malloc failed\n");
    exit(1);
  }

  append(10);
  append(20);
  append(30);

  int first = get(0);

  set(1, 25);

  int count = len;

  append(40);

  insert(0, 15);
  // uncomment either one: the program stops right there and the
  // list below is never printed
  //insert(-1000, 15);
  //insert(10, 15);

  delete_at(1);

  for (int i = 0; i < len; i++) {
    printf("%d ", numbers[i]);
  }
  printf("\n");

  // hand the memory back. the program is about to end anyway and the OS
  // would reclaim it regardless, but in anything longer-lived a missing
  // free() is a leak.
  free(numbers);

  return 0;
}
