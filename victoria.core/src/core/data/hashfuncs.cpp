#include "core/data/hashfuncs.h"

// http://www.cse.yorku.ca/~oz/hash.html
uint32_t hash_djb2(uint8_t *str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}

// https://github.com/skeeto/hash-prospector
uint32_t hash_lowbias32(uint32_t x) {
	x ^= x >> 16;
	x *= 0x7feb352d;
	x ^= x >> 15;
	x *= 0x846ca68b;
	x ^= x >> 16;
	return x;
}

/* clang-format off */
uint32_t PRIMES[] = {
    5,
    11,
    29,
    53,
    97,
    193,
    389,
    769,
    1543,
    3079,
    6151,
    12289,
    24593,
    49157,
    98317,
    196613,
    393241,
    786433,
    1572869,
    3145739,
    6291469,
    12582917,
    25165843,
    50331653,
    100663319,
    201326611,
    402653189,
    805306457,
    1610612741
};
/* clang-format on */

uint32_t PRIMES_SIZE = sizeof(PRIMES) / sizeof(int);