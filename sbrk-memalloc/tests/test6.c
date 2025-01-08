// Test malloc and free in order of allocation and reverse
// writing to all allocated locations with all 1s
// requesting random multiples of 8 bytes up to 2**22

#ifndef DEMO_TEST
#include <malloc.h>
extern int rand(void);
#else
#include <stdlib.h>
#endif

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

int allones = ~0; // allones for int

void *threaded_testmalloc(void *size) {
  int actual_size = *((int *)size);
  void *data = mmap(NULL, actual_size, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (data != MAP_FAILED) {
    memset(data, allones, actual_size);
  }
  return data;
}

int main() {
  fprintf(
      stderr,
      "======================================================================="
      "\n"
      "Allocator stress test with random sizes from the set {1, 8, 8^2, ..., \n"
      " 8^19}. This stress test might get aborted by the OS if your allocator\n"
      "doesn't manage memory properly, because the VM will simply run out of\n"
      "memory.\n"
      "======================================================================="
      "\n");

  pthread_t threads[8];
  int sizes[8];
  int i;

  for (i = 0; i < 100001; i++) {
    for (int j = 0; j < 8; j++) {
      sizes[j] = 8 << (rand() % 20);
      pthread_create(&threads[j], NULL, threaded_testmalloc, &sizes[j]);
    }

    for (int j = 0; j < 8; j++) {
      void *data;
      pthread_join(threads[j], &data);
      munmap(data, sizes[j]);
    }

    // if ((i%100)==0) fprintf(stderr, "%d...", i);
  }
  fprintf(stderr, "malloc random size, free first to last malloc'd %d times.\n",
          i);

  for (i = 0; i < 100001; i++) {
    for (int j = 0; j < 8; j++) {
      sizes[j] = 8 << (rand() % 20);
      pthread_create(&threads[j], NULL, threaded_testmalloc, &sizes[j]);
    }

    for (int j = 7; j >= 0; j--) {
      void *data;
      pthread_join(threads[j], &data);
      munmap(data, sizes[j]);
    }

    // if ((i%100)==0) fprintf(stderr, "%d...", i);
  }
  fprintf(stderr, "malloc random size, free last to first malloc'd %d times.\n",
          i);

  return 0;
}
