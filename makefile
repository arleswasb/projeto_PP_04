# Makefile
CC = gcc
CFLAGS = -O2 -fopenmp -Wall
LDFLAGS = -lm

all: memory_bound cpu_bound

memory_bound: memory_bound.c
	$(CC) $(CFLAGS) -o memory_bound memory_bound.c
	$(CC) $(CFLAGS) -o memory_bound_v1 memory_bound_v1.c
	$(CC) $(CFLAGS) -o memory_bound_v2 memory_bound_v2.c

cpu_bound: cpu_bound.c
	$(CC) $(CFLAGS) -o cpu_bound cpu_bound.c $(LDFLAGS)

clean:
	rm -f memory_bound cpu_bound