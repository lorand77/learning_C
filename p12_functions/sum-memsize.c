#include <stdio.h>

int sum(int a, int b) {
  int c = a + b;
  return c;
}

int main() {
  int s = sum(1, 2);
  printf("sum = %d\n", s);
  
  getchar();  // wait for Enter (no CPU) so ps/top can inspect VSZ and RSS
  
  return 0;
}
