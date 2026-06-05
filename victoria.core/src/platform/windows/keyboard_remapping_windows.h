#pragma once

#include "core/typedefs.h"
#if PLATFORM_WINDOWS

#	include "core/data/hashtable.h"
#	include "core/input/keyboard.h"

class KeyboardRemappingWindows {
	struct HashTableKeyHasher {
		static FORCE_INLINE uint32_t hash(Key p_key) {
			return hash_lowbias32(static_cast<uint32_t>(p_key));
		}

		static FORCE_INLINE uint32_t hash(int p_key) {
			return hash_lowbias32(p_key);
		}
	};

	static HashTable<int, Key, HashTableKeyHasher> scancode_map;
	static HashTable<Key, int, HashTableKeyHasher> inv_scancode_map;

public:
	static void initialize();

	static Key get_key_from_keycode(int p_keycode);
	static int get_keycode_from_key(Key p_key);
};

#endif // PLATFORM_WINDOWS