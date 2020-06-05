
CFLAGS=-std=c11 -O3 -W -Wall -msse4.2

crc32c: crc32c.c

.PHONY: clean

clean:
	rm -f crc32c
