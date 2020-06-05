/*
	Example of calculating crc32c over a string, using all the implementations..

	$ ./crc32c-stdin 123456789
*/
#include <stdio.h>
#include <stdlib.h>

#include "crc32c.c"

int main(int argc, char *argv[]) {
	const char *data = argc > 1 ? argv[1] : "123456789"; // 0xe3069283
	size_t len = strlen(data);

	uint32_t res8 = crc32c_8(~0, data, len);
	printf("crc32c_8 ('%s'): 0x%08x\n", data, res8 ^ ~0);

	uint32_t res32 = crc32c_32(~0, data, len);
	printf("crc32c_32('%s'): 0x%08x\n", data, res32 ^ ~0);

	uint32_t res64 = crc32c_64(~0, data, len);
	printf("crc32c_64('%s'): 0x%08x\n", data, res64 ^ ~0);

	return EXIT_SUCCESS;
}
