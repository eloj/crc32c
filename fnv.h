#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#define FNV_BASIS_32 0x811c9dc5

uint32_t fnv1a_32(uint32_t hash, const void *data, size_t len);

#ifdef __cplusplus
}
#endif
