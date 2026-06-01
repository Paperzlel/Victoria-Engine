#pragma once

#include "core/object/object.h"
#include "core/typedefs.h"

template <typename T>
struct VariantCaster {
	static FORCE_INLINE T cast(const Variant &p_variant) {
		if constexpr (std::is_base_of_v<Object, std::remove_pointer_t<T>>) {
			return Object::cast_to<std::remove_pointer_t<T>>(p_variant);
		} else {
			return p_variant;
		}
	}
};

template <typename T>
struct VariantCaster<T &> {
	static FORCE_INLINE T cast(const Variant &p_variant) {
		if constexpr (std::is_base_of_v<Object, std::remove_pointer_t<T>>) {
			return Object::cast_to<std::remove_pointer_t<T>>(p_variant);
		} else {
			return p_variant;
		}
	}
};

template <typename T>
struct VariantCaster<const T &> {
	static FORCE_INLINE T cast(const Variant &p_variant) {
		if constexpr (std::is_base_of_v<Object, std::remove_pointer_t<T>>) {
			return Object::cast_to<std::remove_pointer_t<T>>(p_variant);
		} else {
			return p_variant;
		}
	}
};

#define VARIANT_CAST_ENUM(m_enum)                                                                                     \
	template <>                                                                                                       \
	struct VariantCaster<m_enum> {                                                                                    \
		static FORCE_INLINE m_enum cast(const Variant &p_variant) {                                                   \
			return (m_enum)p_variant.operator int64_t();                                                              \
		}                                                                                                             \
	}
