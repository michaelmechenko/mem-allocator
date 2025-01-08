CC=gcc
CFLAGS=-g -std=gnu11 -I. -Werror
BINS=mymalloc
TESTS=$(foreach n,1 2 3 4 5 6 7,tests/test$(n) )
DEMO_TESTS=$(foreach n,1 2 3 4 5 6 7,tests/demo_test$(n) )

define \n


endef

.PHONY: all clean test demo

all: mymalloc.o

help:
	@echo \
		"Available make targets: \n\
    make          Compile mymalloc.c to object file, mymalloc.o\n\
    make test     Compile and run tests in the tests directory with mymalloc.\n\
    make demo     Compile and run tests in the tests directory with standard malloc.\n\
    make clean    Clean up all generated files (executables and object files).\n\
    make help     Print available targets"

$(BINS): %: %.o
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(TESTS): CFLAGS:=$(CFLAGS) -Wl,--wrap=sbrk

$(TESTS): %: %.o mymalloc.o sbrk_stats.o
	$(CC) $(CFLAGS) $^ -o $@

test: clean_tests $(TESTS)
	$(foreach t,$(TESTS),$(t)${\n})

$(DEMO_TESTS): tests/demo_%: tests/%.c mymalloc.o
	$(CC) $(CFLAGS) $^ -o $@

demo: CFLAGS:=$(CFLAGS) -DDEMO_TEST

demo: clean_demos $(DEMO_TESTS)
	$(foreach t,$(DEMO_TESTS),$(t)${\n})

clean_demos:
	rm -f $(DEMO_TESTS)

clean: clean_tests clean_demos
	rm -f $(BINS)
	rm -f *.o

clean_tests:
	rm -f tests/*.o
	rm -f $(TESTS)

