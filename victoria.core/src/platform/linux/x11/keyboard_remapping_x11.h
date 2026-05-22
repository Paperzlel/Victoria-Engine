#pragma once

#include "core/typedefs.h"
#if defined(PLATFORM_LINUX) && defined(X11_ENABLED)

#	include "core/data/hashtable.h"
#	include "core/input/keyboard.h"

class KeyboardRemappingX11 {
	struct HashTableKeyHasher {
		static FORCE_INLINE uint32_t hash(Key p_key) {
			return hash_lowbias32(static_cast<uint32_t>(p_key));
		}

		static FORCE_INLINE uint32_t hash(unsigned p_key) {
			return hash_lowbias32(p_key);
		}
	};

	static HashTable<int, Key, HashTableKeyHasher> scancode_map;
	static HashTable<Key, int, HashTableKeyHasher> inv_scancode_map;

public:
	static void initialize();
	static int get_keycode_from_key(Key p_key);
	static Key get_key_from_keycode(int p_key);

	KeyboardRemappingX11() {}
};

#endif // PLATFORM_LINUX && X11_ENABLED
