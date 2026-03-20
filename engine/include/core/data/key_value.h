#pragma once

#include "core/typedefs.h"

template <typename K, typename V>
class KeyValue {
public:
	const K key;
	V value;

	FORCE_INLINE void operator=(const KeyValue &p_kv) {
		const_cast<K &>(key) = p_kv.key;
		value = p_kv.value;
	}

	FORCE_INLINE bool operator==(const KeyValue &p_other) {
		return key == p_other.key && value == p_other.value;
	}
	
	FORCE_INLINE bool operator!=(const KeyValue &p_other) {
		return key != p_other.key && value != p_other.value;
	}

	KeyValue(const KeyValue &p_other) :
		key(p_other.key),
		value(p_other.value) {
	}

	KeyValue(const K &p_key, const V &p_value) :
		key(p_key),
		value(p_value) {
	}
};