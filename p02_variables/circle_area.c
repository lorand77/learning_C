#include <stdio.h>

const double PI = 3.141592653589793;

int main() {
  double r = 2;
  double area = r * r * PI;
  printf("The area of the circle is: %.16f.\n", area);
  return 0;
}
