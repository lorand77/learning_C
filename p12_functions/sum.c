#include <stdio.h>

int sum(int a, int b) {
  int c = a + b;
  return c;
}

// int a[10000000];

int main() {
  int s = sum(1, 2);
  printf("sum = %d\n", s);
  
  // for (int i = 0; i < 5000000; ++i) {  // to test VSZ and RSS
  //   a[i] = i;
  // }

  // for(;;);
  
  return 0;
}
