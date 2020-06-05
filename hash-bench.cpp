/*
	Hashing benchmark. Requires the google benchmark library.
	https://github.com/google/benchmark
*/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#include <benchmark/benchmark.h>
#include "crc32c.h"

static void* read_file(const char *filename, size_t *limit) {
	void *keys = NULL;
	size_t bytes = 0;

	FILE *f = fopen(filename, "rb");
	if (f) {
		fseek(f, 0, SEEK_END);
		bytes = ftell(f);
		fseek(f, 0, SEEK_SET);

		if (*limit > 0 && *limit < bytes)
			bytes = *limit;

		printf("Allocating and reading %zu bytes from '%s'.\n", bytes, filename);
		keys = malloc(bytes);

		long rnum = fread(keys, bytes, 1, f);
		fclose(f);
		if (rnum != 1) {
			free(keys);
			return NULL;
		}
	}

	*limit = bytes;
	return keys;
}

class FileHash : public ::benchmark::Fixture {
public:
	static inline const void *org_data;
	static inline size_t org_size;

	void SetUp(const ::benchmark::State& state) {
		if (!org_data) {
			org_size = 0;
			org_data = static_cast<const uint8_t*>(read_file("hash-bench.cpp", &org_size));
			assert(org_size);
			assert(org_data);
		}
		src = static_cast<const uint8_t*>(org_data);
		len = org_size;
	}

	void UpdateCounters(::benchmark::State &state) {
		uint64_t bytes = state.iterations() * state.range(0);
		state.SetBytesProcessed(bytes);
	}

	const uint8_t *src = NULL;
	size_t len;
};

BENCHMARK_DEFINE_F(FileHash, crc32c_64)(benchmark::State &state) {
	for (auto _ : state) {
		size_t bytes_left = state.range(0);
		uint32_t hash = ~0;

		while (bytes_left) {
			size_t block_len = std::min(len, bytes_left);
			benchmark::DoNotOptimize(hash = crc32c_64(hash, src, block_len));
			bytes_left -= block_len;
		}
	}
	UpdateCounters(state);
}

BENCHMARK_DEFINE_F(FileHash, crc32c_32)(benchmark::State &state) {
	for (auto _ : state) {
		size_t bytes_left = state.range(0);
		uint32_t hash = ~0;

		while (bytes_left) {
			size_t block_len = std::min(len, bytes_left);
			benchmark::DoNotOptimize(hash = crc32c_32(hash, src, block_len));
			bytes_left -= block_len;
		}
	}
	UpdateCounters(state);
}

BENCHMARK_DEFINE_F(FileHash, crc32c_8)(benchmark::State &state) {
	for (auto _ : state) {
		size_t bytes_left = state.range(0);
		uint32_t hash = ~0;

		while (bytes_left) {
			size_t block_len = std::min(len, bytes_left);
			benchmark::DoNotOptimize(hash = crc32c_8(hash, src, block_len));
			bytes_left -= block_len;
		}
	}
	UpdateCounters(state);
}

BENCHMARK_REGISTER_F(FileHash, crc32c_64)->RangeMultiplier(16)->Range(1, 1 << 24);
BENCHMARK_REGISTER_F(FileHash, crc32c_32)->RangeMultiplier(16)->Range(1, 1 << 24);
BENCHMARK_REGISTER_F(FileHash, crc32c_8)->RangeMultiplier(16)->Range(1, 1 << 24);

BENCHMARK_MAIN();
