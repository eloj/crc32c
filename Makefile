OPT=-O3 -fomit-frame-pointer -funroll-loops -fstrict-aliasing -march=native -mtune=native -msse4.2
MISCFLAGS=-fstack-protector -fvisibility=hidden
WARNFLAGS=-W -Wall -Werror -Wno-unused-parameter -Wno-unused-function
CFLAGS=-std=c11 $(OPT) $(MISCFLAGS) $(WARNFLAGS)
CXXFLAGS=-std=gnu++17 -fno-rtti $(OPT) $(MISCFLAGS) $(WARNFLAGS)

all: crc32c-test crc32c-stdin

crc32c-test: crc32c-test.c crc32c.h crc32c.o
	$(CC) $(CFLAGS) $< $(filter %.o, $^) -o $@
crc32c-stdin: crc32c-stdin.c crc32c.h crc32c.o
	$(CC) $(CFLAGS) $< $(filter %.o, $^) -o $@
hash-bench: hash-bench.cpp crc32c.o fnv.o
	$(CXX) $(CXXFLAGS) $< $(filter %.o, $^) -lbenchmark -pthread -o $@

.PHONY: clean bench test

test: crc32c-test
	./crc32c-test

bench: hash-bench
	./hash-bench

clean:
	rm -f fnv.o crc32c.o crc32c-test crc32c-stdin hash-bench
