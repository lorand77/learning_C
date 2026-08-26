#include <stdio.h>

int main() {
 
  int x = 10;
  int y = 100;
 
  if (x > 5) {
    y = 200;
  } else {
    y = 300;
  } 

  printf("y: %d\n", y);
  return 0;
}
