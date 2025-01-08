/**
 * Wrapper for sbrk to collect usage statistics. And printing at the end of
 * a program. Note: calling exit explicitly might skip the stats printing.
 *
 * If compiling without the provided Makefile, use the following gcc options:
 *
 * gcc -g --Wl,--wrap=sbrk std=gnu11 -I. mymalloc.c sbrk_stats.c prog.c -o prog
 */
#include <stdio.h>
#include <unistd.h>

extern void *__real_sbrk(intptr_t increment);

static struct {
  unsigned long added;
  unsigned long returned;
  unsigned long count;
} sbrk_stats = {0};

/** sbrk wrapper */
void *__wrap_sbrk(intptr_t increment) {
  sbrk_stats.count++;
  if (increment >= 0) {
    sbrk_stats.added += increment;
  } else {
    sbrk_stats.returned -= increment;
  }

  return __real_sbrk(increment);
}

// Make stats print automatically after main finishes
void print_stats(void) __attribute__((destructor));

/** Print some statistics about the use of sbrk */
void print_stats() {
  fprintf(stderr,
          "==== sbrk stats ========================\n"
          "Total call count: %lu\n"
          "Total memory added: %lu\n"
          //"Total memory returned: %lu\n"
          "========================================\n",
          sbrk_stats.count, sbrk_stats.added); /*,
                            //sbrk_stats.returned);*/
}
