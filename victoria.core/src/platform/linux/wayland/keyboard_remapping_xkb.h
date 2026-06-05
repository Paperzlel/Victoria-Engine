#pragma once

#include "core/typedefs.h"
#if defined(PLATFORM_LINUX) && defined(WAYLAND_ENABLED)

#	include "core/data/hashtable.h"
#	include "core/input/keyboard.h"

#	include <xkbcommon/xkbcommon.h>

class KeyboardRemappingXKB {
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

	KeyboardRemappingXKB() {}
};

#endif // defined(PLATFORM_LINUX) && defined(WAYLAND_ENABLED)