CC ?= gcc
CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic -O2
.PHONY: all run test clean

all: bellman_ford dijkstra

bellman_ford: src/network.c src/network.h src/bellman_ford.c
	$(CC) $(CFLAGS) src/network.c src/bellman_ford.c -o $@

dijkstra: src/network.c src/network.h src/dijkstra.c
	$(CC) $(CFLAGS) src/network.c src/dijkstra.c -o $@

run: all
	./bellman_ford
	./dijkstra

test: all
	python3 tests/test_results.py

clean:
	rm -f bellman_ford dijkstra
