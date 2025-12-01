CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -g

all: sp_allocator_test

sp_allocator_test: sp_allocator_test.c
	$(CC) $(CFLAGS) -I.. sp_allocator_test.c -o sp_allocator_test

clean:
	rm -f sp_allocator_test

.PHONY: all clean
