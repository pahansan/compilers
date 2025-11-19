#include <stdio.h>

void __attribute__((noinline)) foo(int a) { printf("Hello\n"); }
void __attribute__((noinline)) getpid() { printf("World!\n"); }

int main(int argc, char **argv) {
  foo(0);
  getpid();
  return 0;
}
