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
