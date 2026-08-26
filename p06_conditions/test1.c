#include <stdio.h>

int main() {
  int x = 10;
  int y = 37;

  if (x > y) {
    printf("x is greater than y\n");
  } else if (x == y) {
    printf("x is equal to y\n");
  } else {
    printf("x is not greater than y\n");
  }

  (x > y) ? printf("x is greater than y\n") : printf("x is not greater than y\n"); // too long

  if (x > 10) {
    printf("x is greater than 10\n");
    if (y > 10) {
      printf("y is also greater than 10\n");
    }
  }

  int isLoggedIn = 1;
  int isAdmin = 0;
  int securityLevel = 2; // 1 = highest

  if (isLoggedIn && (isAdmin || securityLevel <= 2) && !(securityLevel < 1)) {
    printf("Access granted\n");
  } else {
    printf("Access denied\n");
  }

  return 0;
}
