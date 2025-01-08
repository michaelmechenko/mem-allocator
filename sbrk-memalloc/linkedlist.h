#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

#include <stddef.h>

// struct for block of memory space
typedef struct mem_block {
  size_t size; // size of block

  struct mem_block *next; // pointer to next address of free space
  int free;               // boolean flag for whether memory is free/not
} mem_block_t;

_LINKEDLIST_H
#endif /* ifndef _LINKEDLIST_H */
