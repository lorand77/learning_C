#include <stdio.h>

#define SIZE 10

int main() {
  int arr[SIZE];
  for (int i = 0; i < SIZE; i++) {
    arr[i] = i+1;
  }

  for (int i = 0; i < SIZE-1; i++) {
    printf("%d, ", arr[i]);
  }
  printf("%d\n", arr[SIZE-1]);

  return 0;
}
