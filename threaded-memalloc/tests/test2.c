// A second test
#ifndef DEMO_TEST
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

void *thread_function(void *arg) {
  for (int i = 0; i < 100; i++) {
    int *data = (int *)malloc(1024);
    free(data);
  }
  return NULL;
}

int main() {

  fprintf(stderr, "============================================================"
                  "===========\n"
                  "This test allocates the same amount of memory repeatedly in "
                  "multiple threads.\n"
                  "sbrk shouldn't be called more than once per thread\n"
                  "============================================================"
                  "===========\n");

  pthread_t threads[10];
  for (int i = 0; i < 10; i++) {
    pthread_create(&threads[i], NULL, thread_function, NULL);
  }

  for (int i = 0; i < 10; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
