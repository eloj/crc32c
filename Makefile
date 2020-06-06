OPT=-O3 -fomit-frame-pointer -funroll-loops -fstrict-aliasing -march=native -mtune=native -msse4.2
MISCFLAGS=-fstack-protector -fvisibility=hidden
WARNFLAGS=-W -Wall -Werror -Wno-unused-parameter -Wno-unused-function
CFLAGS=-std=c11 $(OPT) $(MISCFLAGS) $(WARNFLAGS)
CXXFLAGS=-std=gnu++17 -fno-rtti $(OPT) $(MISCFLAGS) $(WARNFLAGS)

all: crc32c-argv crc32c-stdin

crc32c-argv: crc32c-argv.c crc32c.c crc32c.h
	$(CC) $(CFLAGS) $< -o $@
crc32c-stdin: crc32c-stdin.c crc32c.c crc32c.h
	$(CC) $(CFLAGS) $< -o $@
hash-bench: hash-bench.cpp crc32c.o fnv.o
	$(CXX) $(CXXFLAGS) $+ -lbenchmark -pthread -o $@

.PHONY: clean bench

bench: hash-bench
	./hash-bench

clean:
	rm -f fnv.o crc32c.o crc32c-argv crc32c-stdin hash-bench
