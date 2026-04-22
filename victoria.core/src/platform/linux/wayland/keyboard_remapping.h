#pragma once

#include "core/data/hashtable.h"
#include "core/io/input.h"
#include "core/typedefs.h"

#include <xkbcommon/xkbcommon.h>

class KeyboardRemapping {
	struct HashTableKeyHasher {
		static FORCE_INLINE uint32_t hash(Key p_key) {
			return hash_lowbias32(static_cast<uint32_t>(p_key));
		}

		static FORCE_INLINE uint32_t hash(unsigned p_key) {
			return hash_lowbias32(p_key);
		}
	};

	static HashTable<xkb_keycode_t, Key, HashTableKeyHasher> scancode_map;
	static HashTable<Key, xkb_keycode_t, HashTableKeyHasher> inv_scancode_map;

public:
	static void initialize();

	static xkb_keycode_t get_xkb_keycode_from_key(Key p_key);
	static Key get_key_from_xkb_keycode(xkb_keycode_t p_xkb_key);

	KeyboardRemapping() {}
};
