#ifndef DEMO_TEST
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>

int main() {
  fprintf(stderr, 
      "=======================================================================\n"
      "This test repeatedly allocates 5 differently sized blocks. sbrk should\n" 
      "should not be called more than 25 times in the worst case and your free\n"
      "list should have one or more instance of blocks of 8, 16, 32, 64 and\n"
      "128 bytes\n"
      "=======================================================================\n");


  // Allocate several different uniquely sized blocks
  // Ideally your allocator will be able to re-use many
  // of these blocks.
  for (int i = 0; i < 200; i++) {
    int *data = (int *) malloc(8);
    int *data1 = (int *) malloc(16);
    int *data2 = (int *) malloc(32);
    int *data3 = (int *) malloc(64);
    int *data4 = (int *) malloc(128);

    free(data);
    free(data1);
    free(data2);
    free(data3);
    free(data4);
  }

  return 0;
}
