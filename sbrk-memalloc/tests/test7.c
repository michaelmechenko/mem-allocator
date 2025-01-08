// Test interleaved mallocs and frees
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
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

int allones = ~0; // allones for int

void *thread_safe_malloc(size_t size) {
  void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap failed");
    return NULL;
  }
  return ptr;
}

void thread_safe_free(void *ptr, size_t size) {
  if (munmap(ptr, size) == -1) {
    perror("munmap failed");
  }
}

int *testmalloc(int size) {
  int *data = (int *)thread_safe_malloc(size);
  if (data != NULL) {
    memset((void *)data, allones, size);
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

  int i;
  size_t size;

  for (i = 0; i < 100001; i++) {
    size = 8 << (rand() % 20);
    int *data = (int *)testmalloc(size);
    size_t size1 = 8 << (rand() % 20);
    int *data1 = (int *)testmalloc(size1);
    thread_safe_free(data, size);
    thread_safe_free(data1, size1);
    size_t size2 = 8 << (rand() % 20);
    int *data2 = (int *)testmalloc(size2);
    size_t size3 = 8 << (rand() % 20);
    int *data3 = (int *)testmalloc(size3);
    thread_safe_free(data2, size2);
    thread_safe_free(data3, size3);
    size_t size4 = 8 << (rand() % 20);
    int *data4 = (int *)testmalloc(size4);
    thread_safe_free(data4, size4);
    size_t size5 = 8 << (rand() % 20);
    int *data5 = (int *)testmalloc(size5);
    size_t size6 = 8 << (rand() % 20);
    int *data6 = (int *)testmalloc(size6);
    thread_safe_free(data5, size5);
    thread_safe_free(data6, size6);
    size_t size7 = 8 << (rand() % 20);
    int *data7 = (int *)testmalloc(size7);
    thread_safe_free(data7, size7);
  }
  fprintf(stderr, "malloc random size, interleaved frees %d times.\n", i - 1);

  return 0;
}
