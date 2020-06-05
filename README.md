# CRC32c using SSE4.2

A few examples of how to use compiler built-ins to use the CRC32 instructions
provided by SSE4.2 on IA32/AMD64 CPUs.

The short story is that there are separate instructions for updating a CRC
using 8-, 16-, 32- and 64-bits of input at a time.

A typical implementation first consumes as many bytes as possible using
the widest instruction available, then falls back to the narrower ones as appropriate.

The details of exactly how this is done matters for throughput, which is why this
repository includes a benchmark.

## Why

Most people probably do _not_ have a use for CRC32c specifically, but a CRC can
be used as a general hash function too in some cases, _except_ where there's a risk
for [hash-flooding](https://www.google.com/search?q=hash-flooding) attacks of course.

Sometimes you just want something simple that you can quickly type or copy and paste into
your code to get going, that's when this may be appropriate.

For comparison, I've included a naïve implementation of the 32-bit [fnv-1a hash](https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function).
This hash is often recommended because it's very simple to implement, but in practice
it is also slower than even the slowest CRC32c version, which is also trivial,
never mind the one that consumes 8-bytes at a time:

```
FileHash/crc32c_64/16             6.67 ns         6.67 ns    104924455 bytes_per_second=2.23387G/s
FileHash/crc32c_64/256            14.9 ns         14.9 ns     46896722 bytes_per_second=15.9996G/s
FileHash/crc32c_64/4096            465 ns          465 ns      1505399 bytes_per_second=8.20575G/s
FileHash/crc32c_64/65536          7753 ns         7753 ns        90290 bytes_per_second=7.87262G/s
FileHash/crc32c_64/1048576      124573 ns       124566 ns         5630 bytes_per_second=7.83971G/s
FileHash/crc32c_64/16777216    1989719 ns      1989606 ns          352 bytes_per_second=7.85332G/s
...
FileHash/fnv1a_32/16              10.4 ns         10.4 ns     67782311 bytes_per_second=1.43716G/s
FileHash/fnv1a_32/256              294 ns          294 ns      2379275 bytes_per_second=830.064M/s
FileHash/fnv1a_32/4096            4952 ns         4952 ns       141357 bytes_per_second=788.885M/s
FileHash/fnv1a_32/65536          79484 ns        79473 ns         8806 bytes_per_second=786.43M/s
FileHash/fnv1a_32/1048576      1272001 ns      1271820 ns          550 bytes_per_second=786.275M/s
FileHash/fnv1a_32/16777216    20350940 ns     20348028 ns           34 bytes_per_second=786.317M/s
```

Something like [xxhash](https://github.com/Cyan4973/xxHash) will be faster still, but now we've left
the domain of "easy to just copy'n'paste or implement from scratch" behind.

## Alternative implementations

I've provided `crc32c_64` which consumes blocks of 64-bits, then a 32-bit word
if required, and then falls back to individual octets. This is generally the
fastest version.

In the middle is `crc32c_32` which consumes 32-bit words before falling back
to octets.

Last we have `crc32c_8` which simply processes the data one byte at
a time. This is almost certainly the slowest version, unless you are
hashing 16 bytes or less, but it's also very simple to just copy'n'paste
and get going if you don't care about performance.

One could also take `crc32c_64` and remove the 32-bit update (and associated conditional),
and extend the 'duff-style' switch to process up to seven octets.

I can heartily recommend pasting functions into the [compiler explorer](https://godbolt.org),
comparing the assembly output of the different functions on different compilers. Just remember to add optimization flags.

## Features

* Uses `memcpy()` instead of casts to avoid UB; no alignment issues.

## Benchmark

Issue `make bench` to run it.

This code is built on top of, and requires the prior installation of the [Google benchmark](https://github.com/google/benchmark) microbenchmark support library,
which is available as `libbenchmark-dev` on Debian and Ubuntu.

## Usage example

```c
#include <stdio.h>
#include <stdlib.h>

#include "crc32c.c"

int main(int argc, char *argv[]) {
	const char *data = "123456789"; // -> 0xe3069283
	size_t len = strlen(data);

	uint32_t crc = ~0;
	crc = crc32c_64(crc, data, len);
	printf("crc32c('%s'): 0x%08x\n", data, crc ^ ~0);

	return EXIT_SUCCESS;
}
```
