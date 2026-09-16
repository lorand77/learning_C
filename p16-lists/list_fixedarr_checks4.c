// Same as list_fixedarr_checks3.c, but the capacity is a command line
// argument instead of a compile time constant:
//
//   ./list_fixedarr_checks4.bin 16
//
// This is the thing malloc makes possible. A global "int numbers[CAP]"
// needs its size known while compiling; here the size arrives while the
// program is already running, so CAP becomes an ordinary variable "cap".

#include <stdio.h>
#include <stdlib.h>

// just a pointer now: no memory attached to it until main calls malloc.
// using it before that would read a wild address.
int *numbers;
int len = 0;
int cap = 0;   // was #define CAP -- now filled in from argv

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
  if (len >= cap) {
    fprintf(stderr, "append: list is full (cap=%d)\n", cap);
    exit(1);
  }
  numbers[len] = value;
  len++;
}

// pos == len is allowed here: it means "insert at the end"
void insert(int pos, int value) {
  if (len >= cap) {
    fprintf(stderr, "insert: list is full (cap=%d)\n", cap);
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

// argc counts the arguments including the program name, so argv[0] is
// "./list_fixedarr_checks4.bin" and argv[1] is the first real argument.
int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <capacity>\n", argv[0]);
    exit(1);
  }

  // argv is always text: "16" arrives as the characters '1' and '6', not
  // as a number. strtol converts it and reports where it stopped reading,
  // which is how a typo like "16x" or "abc" gets caught.
  char *rest;
  long n = strtol(argv[1], &rest, 10);
  if (*rest != '\0') {
    fprintf(stderr, "capacity must be a number, got \"%s\"\n", argv[1]);
    exit(1);
  }
  if (n < 1 || n > 1000000) {
    fprintf(stderr, "capacity must be between 1 and 1000000, got %ld\n", n);
    exit(1);
  }
  cap = (int)n;

  // ask for room for cap ints. sizeof(int) matters: malloc counts bytes,
  // not elements, so cap alone would reserve a quarter of what is needed.
  numbers = malloc((size_t)cap * sizeof(int));

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
