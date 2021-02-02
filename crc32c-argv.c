/*
	Example of calculating crc32c over a string, using all the implementations..

	$ ./crc32c-argv 123456789
*/
#include <stdio.h>
#include <stdlib.h>

#include "crc32c.c"

int main(int argc, char *argv[]) {
	const char *data = argc > 1 ? argv[1] : "123456789"; // 0xe3069283
	size_t len = strlen(data);

	uint32_t allones = ~(uint32_t)0;
	uint32_t crc;

	crc = crc32c_8(~0, data, len);
	printf("crc32c_8  ('%s'): 0x%08x\n", data, crc ^ allones);

	crc = crc32c_32(~0, data, len);
	printf("crc32c_32 ('%s'): 0x%08x\n", data, crc ^ allones);

	crc = crc32c_64(~0, data, len);
	printf("crc32c_64 ('%s'): 0x%08x\n", data, crc ^ allones);

	crc32_initialize();
	crc = crc32c_tbl(~0, data, len);
	printf("crc32c_tbl('%s'): 0x%08x\n", data, crc ^ allones);

	return EXIT_SUCCESS;
}
