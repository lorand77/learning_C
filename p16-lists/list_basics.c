// The same operations as list_basics.py, in C.
//
// A Python list is a growable array. The simplest C version of that is
// a fixed-size array plus a count of how many slots are actually used.
// Nothing here checks bounds: that is the programmer's job in C.

#include <stdio.h>

#define CAP 16      // how many elements fit

int numbers[CAP];   // the storage
int len = 0;        // how many of them are in use

void print_list(const char *label) {
  printf("%s [", label);
  for (int i = 0; i < len; i++) {
    printf("%d", numbers[i]);
    if (i < len - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

// add to the end
void append(int value) {
  numbers[len] = value;
  len++;
}

// add at a position, shifting the rest right
// (copy from the back, otherwise each value overwrites the next one)
void insert(int pos, int value) {
  for (int i = len; i > pos; i--) {
    numbers[i] = numbers[i - 1];
  }
  numbers[pos] = value;
  len++;
}

// remove a position, shifting the rest left
void delete_at(int pos) {
  for (int i = pos; i < len - 1; i++) {
    numbers[i] = numbers[i + 1];
  }
  len--;
}

int main() {
  // create
  append(10);
  append(20);
  append(30);
  print_list("list:");

  // read (indexes start at 0)
  printf("first element: %d\n", numbers[0]);

  // write
  numbers[1] = 25;
  print_list("after write:");

  // count -- in Python len() asks the list, here we track it ourselves
  printf("number of elements: %d\n", len);

  // append (add to the end)
  append(40);
  print_list("after append:");

  // insert (add at a position, shifts the rest right)
  insert(0, 15);
  print_list("after insert:");

  // delete by index
  delete_at(0);
  print_list("after delete:");

  return 0;
}
