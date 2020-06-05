#include "fnv.h"

uint32_t fnv1a_32(uint32_t hash, const void *data, size_t len) {
	const uint8_t *src = data;

	for (size_t i=0 ; i < len ; ++i) {
		hash ^= src[i];
		hash *= 0x01000193;
	}

	return hash;
}

#if 0
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
	char buf[4096];
	ssize_t len;

	uint32_t hash = FNV_BASIS_32;

	while ((len = read(0, buf, sizeof(buf))) > 0) {
		hash = fnv1a_32(hash, buf, len);
	}

	printf("fnv1a: 0x%08x\n", hash);

	return EXIT_SUCCESS;
}
#endif
