/*
	Example of calculating crc32c over a buffer.

	$ ./crc32c-stdin < crc32-stdin.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "crc32c.h"

int main(int argc, char *argv[]) {
	char buf[4096];
	ssize_t len;

	uint32_t crc = ~(uint32_t)0;

	while ((len = read(0, buf, sizeof(buf))) > 0) {
		crc = crc32c(crc, buf, len);
	}

	printf("crc32c: 0x%08x\n", crc ^ ~(uint32_t)0);

	return EXIT_SUCCESS;
}
