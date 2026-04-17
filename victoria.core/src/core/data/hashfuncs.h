#pragma once

#include "core/typedefs.h"

VAPI uint32_t hash_djb2(uint8_t *str);
VAPI uint32_t hash_lowbias32(uint32_t x);

class HasherDefault {
public:
	static FORCE_INLINE uint32_t hash(uint8_t *p_key);
	static FORCE_INLINE uint32_t hash(const char *p_key);
	static FORCE_INLINE uint32_t hash(uint32_t p_key);
};

uint32_t HasherDefault::hash(uint8_t *p_key) {
	return hash_djb2(p_key);
}

uint32_t HasherDefault::hash(const char *p_key) {
	return hash_djb2((uint8_t *)p_key);
}

uint32_t HasherDefault::hash(uint32_t p_key) {
	return hash_lowbias32(p_key);
}

VAPI extern uint32_t PRIMES[];
VAPI extern uint32_t PRIMES_SIZE;