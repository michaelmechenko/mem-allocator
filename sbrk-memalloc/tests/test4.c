// Increasing size test using calloc
// check each location is set to 0

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int allones = ~0; // allones for int

void *test_alloc(void *arg) {
  size_t size = *(size_t *)arg;
  fprintf(stderr, "%zu bytes...", size);
  int *data = (int *)calloc(1, size);
  if (data != NULL) {
    for (int j = 0; j < size / sizeof(int); ++j) {
      assert(data[j] == 0);
    }
    free(data);
  } else {
    fprintf(stderr, "\nMax size allocated: %lu bytes\n", size / 2);
    pthread_exit(NULL);
  }
  return NULL;
}

int main() {

  int i;
  size_t size;
  pthread_t threads[28];

  fprintf(
      stderr,
      "======================================================================="
      "\n"
      "This test uses calloc to allocate gradually allocate 2^2, 2^3, ...,\n"
      "2^29 bytes of memory. It checks whether the memory is set to 0. No "
      "more\n"
      "than 28 calls to mmap should be made.\n"
      "======================================================================="
      "\n");

  for (i = 2; i < 30; ++i) {
    size = 2UL << i;
    if (pthread_create(&threads[i - 2], NULL, test_alloc, &size) != 0) {
      fprintf(stderr, "Failed to create thread for size %zu bytes\n", size);
      break;
    }
    pthread_join(threads[i - 2], NULL);
  }
  fprintf(stderr, "\n");
  return 0;
}
