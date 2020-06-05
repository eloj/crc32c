/*
	CRC32c using ia32/AMD64 hardware instructions.

	To compare with test vectors, the initial CRC should be -1, and you need a final xor with -1.

	NOTE: The crc32 instructions are part of SSE4.2

	$ gcc -std=c11 -O3 -msse4.2 crc32c.c -o crc32c
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

__attribute__ ((target ("sse4.2")))
uint32_t crc32c_8(uint32_t crc, const void *data, size_t len) {
	const uint8_t *src = data;

	for (size_t i=0 ; i < len ; ++i) {
		crc = __builtin_ia32_crc32qi(crc, *src++);
	}

	return crc;
}

__attribute__ ((target ("sse4.2")))
uint32_t crc32c_32(uint32_t crc, const void *data, size_t len) {
	const uint8_t *src = data;

	for (size_t i=0 ; i < len >> 2; ++i) {
		uint32_t buf;
		memcpy(&buf, src, sizeof(buf));
		crc = __builtin_ia32_crc32si(crc, buf);
		src += sizeof(buf);
	}

	switch (len & 3) {
		case 3:
			crc = __builtin_ia32_crc32qi(crc, *src++);
			/* fallthrough */
		case 2:
			crc = __builtin_ia32_crc32qi(crc, *src++);
			/* fallthrough */
		case 1:
			crc = __builtin_ia32_crc32qi(crc, *src);
	}

	return crc;
}

__attribute__ ((target ("sse4.2")))
uint32_t crc32c_64(uint32_t crc, const void *data, size_t len) {
	const uint8_t *src = data;

	// Handle input in units of 64 bits.
	for (size_t i=0 ; i < len >> 3; ++i) {
		uint64_t buf;
		memcpy(&buf, src, sizeof(buf));
		crc = __builtin_ia32_crc32di(crc, buf);
		src += sizeof(buf);
	}

	// Handle tail block of 32 bits.
	if ((len & 7) >> 2) {
		uint32_t buf;
		memcpy(&buf, src, sizeof(buf));
		crc = __builtin_ia32_crc32si(crc, buf);
		src += sizeof(buf);
	}

	// Handle tail of input in units if 8 bits.
	switch (len & 3) {
		case 3:
			crc = __builtin_ia32_crc32qi(crc, *src++);
			/* fallthrough */
		case 2:
			crc = __builtin_ia32_crc32qi(crc, *src++);
			/* fallthrough */
		case 1:
			crc = __builtin_ia32_crc32qi(crc, *src);
	}

	return crc;
}

#ifndef NO_CRC32C_MAIN
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
#endif
