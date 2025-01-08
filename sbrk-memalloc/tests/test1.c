// Basic test

#ifndef DEMO_TEST
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/mman.h>

#define ARRAY_ELEMENTS 1024

void *thread_safe_malloc(size_t size) {
  void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    return NULL;
  }
  return ptr;
}

void thread_safe_free(void *ptr, size_t size) { munmap(ptr, size); }

int main() {
  fprintf(
      stderr,
      "======================================================================="
      "\n"
      "This test should only call mmap once to request %lu bytes for the\n"
      "memory returned to the user + however many bytes you need for your\n"
      "block struct. You should not get any memory error or assertion error.\n"
      "======================================================================="
      "\n",
      ARRAY_ELEMENTS * sizeof(int));

  // Allocate some data
  int *data = (int *)thread_safe_malloc(ARRAY_ELEMENTS * sizeof(int));

  int *old_ptr = data;

  // Do something with the data
  int i = 0;
  for (i = 0; i < ARRAY_ELEMENTS; i++) {
    data[i] = i;
  }

  // Free the data
  thread_safe_free(data, ARRAY_ELEMENTS * sizeof(int));
  data = NULL;

  // allocate again - we should get the same block
  data = (int *)thread_safe_malloc(ARRAY_ELEMENTS * sizeof(int));

  assert(data == old_ptr);
  old_ptr = data;

  thread_safe_free(data, ARRAY_ELEMENTS * sizeof(int));
  data = NULL;

  // allocate a smaller chunk - we should still get the same block
  data = (int *)thread_safe_malloc(sizeof(int));

  assert(data == old_ptr);

  thread_safe_free(data, sizeof(int));

  return 0;
}
