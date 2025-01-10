#ifndef DEMO_TEST
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <pthread.h>
#include <stdio.h>

void *thread_func(void *arg) {
  for (int i = 0; i < 200; i++) {
    int *data = (int *)malloc(8);
    int *data1 = (int *)malloc(16);
    int *data2 = (int *)malloc(32);
    int *data3 = (int *)malloc(64);
    int *data4 = (int *)malloc(128);

    free(data);
    free(data1);
    free(data2);
    free(data3);
    free(data4);
  }
  return NULL;
}

int main() {
  fprintf(
      stderr,
      "======================================================================="
      "\n"
      "This test repeatedly allocates 5 differently sized blocks using mmap.\n"
      "sbrk should not be called. This test uses pthreads to ensure thread-\n"
      "safety of the malloc implementation. Each thread should have its own\n"
      "instance of blocks of 8, 16, 32, 64, and 128 bytes.\n"
      "======================================================================="
      "\n");

  pthread_t threads[10];
  for (int i = 0; i < 10; i++) {
    pthread_create(&threads[i], NULL, thread_func, NULL);
  }

  for (int i = 0; i < 10; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
