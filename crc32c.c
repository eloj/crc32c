/*
	CRC32c using ia32/AMD64 SSE4.2 hardware instructions.
	Copyright 2020, 2021 Eddy L O Jansson. Licensed under The MIT License.

	Three different examples, using 8-, 32- and 64-bit versions of the CRC32c CPU instructions.
	Just copy'n'paste the version you want.

	Recommend to inspect generated code for individual functions using https://godbolt.org

	To compare with external test vectors, the initial CRC should be -1, and you need a final xor with -1.

	$ gcc -std=c11 -O3 -msse4.2 crc32c.c
*/
#include "crc32c.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Global table for software fallback ('reversed' for right-shifts)
static uint32_t crc32c_table[256];

typedef uint32_t(*crc32c_func_t)(uint32_t crc, const void *data, size_t len);

/*
	Calculate the CRC32c of a block of memory, in 8-bit units.
*/
__attribute__ ((target ("sse4.2")))
uint32_t crc32c_8(uint32_t crc, const void *data, size_t len) {
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

/*
	Calculate the CRC32c of a block of memory, in 64-bit units.
*/
__attribute__ ((target ("sse4.2")))
uint32_t crc32c_64(uint32_t crc, const void *data, size_t len) {
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

uint32_t crc32c_soft(uint32_t crc, const void *data, size_t len) {
	assert(crc32c_table[1] == 0xf26b8303);
	unsigned char const *p = data;
	while (len--)
		crc = crc32c_table[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
	return crc;
}

static void crc32_build_table(uint32_t *table, uint32_t inv_poly) {
	for (int i=0 ; i < 256 ; ++i) {
		uint32_t res = i;
		for (int j=0 ; j < 8 ; ++j) {
			res = (res & 1) ? (res >> 1) ^ inv_poly : (res >> 1);
		}
		crc32c_table[i] = res;
	}
}

void crc32c_initialize() {
	crc32_build_table(crc32c_table, 0x82F63B78); // 0x1EDC6F41 bit-reversed
}

static crc32c_func_t runtime_resolve_crc32c(void) {
	__builtin_cpu_init();
	if (__builtin_cpu_supports("sse4.2"))
		return crc32c_64;
	crc32c_initialize();
	return crc32c_soft;
}

uint32_t crc32c(uint32_t crc, const void *data, size_t len) __attribute__ ((ifunc ("runtime_resolve_crc32c")));
