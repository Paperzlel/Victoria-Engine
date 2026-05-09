#pragma once

#include "core/typedefs.h"

VAPI uint32_t hash_djb2(uint8_t *str);
VAPI uint32_t hash_lowbias32(uint32_t x);

// Taken from Godot. Super useful generic we should probably keep using.

template <typename, typename = std::void_t<>>
struct has_hash_method : std::false_type {};

template <typename T>
struct has_hash_method<T, std::void_t<std::is_same<decltype(std::declval<const T>().hash()), uint32_t>>>
	: std::true_type {};

template <typename T>
constexpr bool has_hash_method_v = has_hash_method<T>::value;

template <typename T, typename = void>
struct HasherDefaultImpl {};

struct HasherDefault {
public:
	template <typename T>
	static FORCE_INLINE uint32_t hash(const T &p_key) {
		return HasherDefaultImpl<std::decay_t<T>>::hash(p_key);
	}
};

template <typename T>
struct HasherDefaultImpl<T, std::enable_if_t<has_hash_method_v<T>>> {
	static FORCE_INLINE uint32_t hash(const T &p_value) {
		return p_value.hash();
	}
};

template <>
struct HasherDefaultImpl<char *> {
	static FORCE_INLINE uint32_t hash(const char *p_str) {
		return hash_djb2((uint8_t *)p_str);
	}
};

template <>
struct HasherDefaultImpl<uint32_t> {
	static FORCE_INLINE uint32_t hash(uint32_t p_value) {
		return hash_lowbias32(p_value);
	}
};

template <>
struct HasherDefaultImpl<uint16_t> {
	static FORCE_INLINE uint32_t hash(uint32_t p_value) {
		return hash_lowbias32(p_value);
	}
};

template <>
struct HasherDefaultImpl<uint8_t> {
	static FORCE_INLINE uint32_t hash(uint32_t p_value) {
		return hash_lowbias32(p_value);
	}
};

template <>
struct HasherDefaultImpl<int8_t> {
	static FORCE_INLINE uint32_t hash(uint32_t p_value) {
		return hash_lowbias32(p_value);
	}
};

template <>
struct HasherDefaultImpl<int16_t> {
	static FORCE_INLINE uint32_t hash(uint32_t p_value) {
		return hash_lowbias32(p_value);
	}
};

template <>
struct HasherDefaultImpl<int32_t> {
	static FORCE_INLINE uint32_t hash(uint32_t p_value) {
		return hash_lowbias32(p_value);
	}
};

VAPI extern uint32_t PRIMES[];
VAPI extern uint32_t PRIMES_SIZE;