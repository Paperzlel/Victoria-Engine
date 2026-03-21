#pragma once

#include "core/typedefs.h"

VAPI u32 hash_djb2(u8 *str);
VAPI u32 hash_lowbias32(u32 x);

class HasherDefault {
public:
	static FORCE_INLINE u32 hash(u8 *p_key);
	static FORCE_INLINE u32 hash(const char *p_key);
	static FORCE_INLINE u32 hash(u32 p_key);
};

u32 HasherDefault::hash(u8 *p_key) {
	return hash_djb2(p_key);
}

u32 HasherDefault::hash(const char *p_key) {
	return hash_djb2((u8 *)p_key);
}

u32 HasherDefault::hash(u32 p_key) {
	return hash_lowbias32(p_key);
}

extern int PRIMES[];
extern int PRIMES_SIZE;