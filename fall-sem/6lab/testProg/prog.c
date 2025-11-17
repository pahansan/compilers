#include <stdio.h>

void __attribute__((noinline)) foo(int a) { printf("Hello\n"); }

int main(int argc, char **argv) {
  foo(1);
  return 0;
}
