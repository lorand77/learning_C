#include <stdio.h>

int main() {
  // char msg[] = "ab";
  // char msg[] = {'a', 'b', '\0'};
  // char msg[] = {97, 98, 0};

  // char msg[] = {97, 98};    // not good
  //char msg2[] = "secret";

  // char msg[] = "😀";
  // unsigned char msg[] ={0xF0, 0x9F, 0x98, 0x80, 0}; // UTF-8 encoding of 😀
  
  char msg[] = "Hi";
  // msg[1] = 'a';
  printf("%s\n", msg);
  printf("size = %zu\n", sizeof(msg));

  for (long unsigned int i = 0; i < sizeof(msg) - 1; ++i) {
    printf("%c\n", msg[i]);
  }
  return 0;
}
