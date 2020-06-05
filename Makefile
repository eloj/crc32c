
CFLAGS=-std=c11 -O3 -W -Wall -msse4.2 -Wno-unused-parameter -Wno-unused-function

all: crc32c-argv crc32c-stdin

crc32c-argv: crc32c-argv.c
crc32c-stdin: crc32c-stdin.c

.PHONY: clean

clean:
	rm -f crc32c-argv crc32c-stdin
