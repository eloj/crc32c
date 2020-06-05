#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

uint32_t crc32c_8(uint32_t crc, const void *data, size_t len);
uint32_t crc32c_32(uint32_t crc, const void *data, size_t len);
uint32_t crc32c_64(uint32_t crc, const void *data, size_t len);

#ifdef __cplusplus
}
#endif
