/*
	Basic test for crc32c functions.

	$ ./crc32c-test
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "crc32c.h"

int main(int argc, char *argv[]) {
	const char *data = "0123456789abcdef";
	size_t len = strlen(data);

	uint32_t allones = ~(uint32_t)0;
	uint32_t crc, crc_first;

	// Test easiest hw implementation
	crc_first = crc = crc32c_8(~0, data, len);
	printf("crc32c_8  ('%s'): 0x%08x\n", data, crc ^ allones);

	// Test 32-bit input version
	crc = crc32c_32(allones, data, len);
	printf("crc32c_32 ('%s'): 0x%08x\n", data, crc ^ allones);
	assert(crc == crc_first);

	// Test 64-bit input version
	crc = crc32c_64(allones, data, len);
	printf("crc32c_64 ('%s'): 0x%08x\n", data, crc ^ allones);
	assert(crc == crc_first);

	// Test software implementation
	crc32c_initialize();
	crc = crc32c_soft(allones, data, len);
	printf("crc32c_soft('%s'): 0x%08x\n", data, crc ^ allones);
	assert(crc == crc_first);

	// Test rolling
	crc = crc32c(allones, data, len / 2);
	crc = crc32c(crc, data + (len / 2), len - (len / 2));
	printf("crc32c_64 rolling('%s'): 0x%08x\n", data, crc ^ allones);
	assert(crc == crc_first);

	assert((crc ^ allones) == 0x42d3119e);

	return EXIT_SUCCESS;
}
