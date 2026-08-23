#include <stdio.h>

int main() {
  int x = 5;
  int y = 3;
  printf("%d\n", x == y);
  if (x == y) {
    printf("x is equal to y\n");
  } else {
    printf("x is not equal to y\n");
  }
  printf("x=%d\n", x);
  return 0;
}
