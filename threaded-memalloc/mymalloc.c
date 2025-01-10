#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include "debug.h" // definition of debug_printf
#include "linkedlist.h"
#include "malloc.h"
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// define page size
#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

// define a global mutex
pthread_mutex_t malloc_mutex = PTHREAD_MUTEX_INITIALIZER;

// global variable for first block of mem
mem_block_t *head = NULL;

void *mymalloc(size_t s) {
  assert(s > 0);                     // verify size > 0
  pthread_mutex_lock(&malloc_mutex); // create mutex lock

  // initialize pointers for list
  mem_block_t *current = head;
  mem_block_t *prev = NULL;

  // traverse list to find space of apt size (first-fit)
  while (current != NULL) {
    if (current->free && current->size >= s) {
      if (current->size > s + sizeof(mem_block_t)) {
        // split block and allocate space
        mem_block_t *new_block =
            (mem_block_t *)((char *)current + sizeof(mem_block_t) + s);
        new_block->size = current->size - s - sizeof(mem_block_t);
        new_block->free = 1;
        new_block->next = current->next;
        current->size = s;
        current->next = new_block;
      }
      current->free = 0;
      debug_printf("Malloc %zu bytes\n", s);
      return (char *)current + sizeof(mem_block_t);
    }
    prev = current; // move to next block
    current = current->next;
  }

  // calculate total size including supplemental data
  size_t total_size = s + sizeof(mem_block_t);

  // request more memory
  // task 1: use mmap instead of sbrk
  mem_block_t *block;
  if (total_size < PAGE_SIZE) {
    block = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  } else {
    size_t pages = (total_size + PAGE_SIZE - 1) / PAGE_SIZE;
    block = mmap(NULL, pages * PAGE_SIZE, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  }

  if (block == MAP_FAILED) {
    return NULL;
  }

  // initialize new memory block
  block->size = s;
  block->next = NULL;
  block->free = 0;

  // link new block to list
  if (prev == NULL) {
    head = block;
  } else {
    prev->next = block;
  }

  debug_printf("Malloc %zu bytes\n", s);

  // return usable memory address & unlock mutex
  pthread_mutex_unlock(&malloc_mutex);
  return (char *)block + sizeof(mem_block_t);
}

void *mycalloc(size_t nmemb, size_t s) {
  // verify inputs > 0 & initiate mutexes
  assert(nmemb > 0 && s > 0);
  pthread_mutex_lock(&malloc_mutex);

  // calculate total size
  size_t total_size = nmemb * s;

  // allocate memory
  void *ptr = mymalloc(total_size);

  // initialize allocated memory to zero
  if (ptr) {
    memset(ptr, 0, total_size);
  }
  debug_printf("Calloc %zu bytes\n", total_size);

  // return allocated memory & unlock mutex
  pthread_mutex_unlock(&malloc_mutex);
  return ptr;
}

// coalesce blocks
void coalesce_free_blocks() {
  mem_block_t *current = head;
  // if next block is not empty and both current
  // and next are free, combine and change
  // pointer to following block
  while (current && current->next) {
    if (current->free && current->next->free) {
      current->size += sizeof(mem_block_t) + current->next->size;
      current->next = current->next->next;
    } else {
      current = current->next;
    }
  }
}

void myfree(void *ptr) {
  assert(ptr != NULL);

  // initiate mutex lock
  pthread_mutex_lock(&malloc_mutex);

  // get necessary data
  mem_block_t *block = (mem_block_t *)((char *)ptr - sizeof(mem_block_t));

  // mark block as free and coalesce or unmap
  if (block->size >= PAGE_SIZE) {
    munmap(block, block->size + sizeof(mem_block_t));
  } else {
    block->free = 1;
    coalesce_free_blocks();
  }
  debug_printf("Freed %zu bytes\n", block->size);

  // unlock mutex
  pthread_mutex_unlock(&malloc_mutex);
}
