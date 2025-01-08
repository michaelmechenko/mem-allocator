// Test malloc and free in order of allocation and reverse
// writing to all allocated locations with all 1s
// requesting ordered multiples of 8 bytes up to 2**10

#ifndef DEMO_TEST
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <pthread.h>
#include <stdio.h>
#include <string.h>

int allones = ~0; // allones for int

void *threaded_testmalloc(void *size) {
  int sz = *(int *)size;
  int *data = (int *)malloc(sz);
  if (data != NULL) {
    memset((void *)data, allones, sz);
  }
  pthread_exit(data);
}

int main() {
  int i;
  pthread_t threads[8];
  int sizes[8] = {8, 16, 32, 64, 128, 256, 512, 1024};

  fprintf(stderr, "============================================================"
                  "===========\n"
                  "Allocator stress test using pthreads and mmap. There "
                  "shouldn't be more than 64 calls to sbrk in\n"
                  "the worst case scenario.\n"
                  "============================================================"
                  "===========\n");

  for (i = 0; i < 1000; i++) {
    for (int j = 0; j < 8; j++) {
      pthread_create(&threads[j], NULL, threaded_testmalloc, (void *)&sizes[j]);
    }

    for (int j = 0; j < 8; j++) {
      int *data;
      pthread_join(threads[j], (void **)&data);
      free(data);
    }
  }
  fprintf(
      stderr,
      "malloc small to large, free small to large %d times using pthreads.\n",
      i);

  for (i = 0; i < 1000; i++) {
    for (int j = 0; j < 8; j++) {
      pthread_create(&threads[j], NULL, threaded_testmalloc, (void *)&sizes[j]);
    }

    for (int j = 7; j >= 0; j--) {
      int *data;
      pthread_join(threads[j], (void **)&data);
      free(data);
    }
  }
  fprintf(
      stderr,
      "malloc small to large, free large to small %d times using pthreads.\n",
      i);

  return 0;
}
