/*
	CRC32c using ia32/AMD64 SSE4.2 hardware instructions.
	(C)2020 Eddy L O Jansson. Licensed under The MIT License.

	Three different examples, using 8-, 32- and 64-bit versions of the CRC32c CPU instructions.
	Just copy'n'paste the version you want.

	Note the use of memcpy() to avoid issues with undefined behaviour (UB) due to alignment
	or type punning. The compiler will elide it when optimizing.

	Recommend to inspect generated code for individual functions using https://godbolt.org

	To compare with external test vectors, the initial CRC should be -1, and you need a final xor with -1.

	$ gcc -std=c11 -O3 -msse4.2 crc32c.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/*
	Calculate the CRC32c of a block of memory, in 8-bit units.
*/
__attribute__ ((target ("sse4.2")))
static uint32_t crc32c_8(uint32_t crc, const void *data, size_t len) {
	const uint8_t *src = data;

	for (size_t i=0 ; i < len ; ++i) {
		crc = __builtin_ia32_crc32qi(crc, *src++);
	}

	return crc;
}

/*
	Calculate the CRC32c of a block of memory, in 32-bit units.
*/
__attribute__ ((target ("sse4.2")))
static uint32_t crc32c_32(uint32_t crc, const void *data, size_t len) {
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

/*
	Calculate the CRC32c of a block of memory, in 64-bit units.
*/
__attribute__ ((target ("sse4.2")))
static uint32_t crc32c_64(uint32_t crc, const void *data, size_t len) {
	const uint8_t *src = data;

	for (size_t i=0 ; i < len >> 3; ++i) {
		uint64_t buf;
		memcpy(&buf, src, sizeof(buf));
		crc = __builtin_ia32_crc32di(crc, buf);
		src += sizeof(buf);
	}

	if ((len & 7) >> 2) {
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
