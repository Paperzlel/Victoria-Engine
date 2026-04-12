#pragma once

#include "variant.h"

#include "core/typedefs.h"

template <typename T>
struct VariantCaster {
	static FORCE_INLINE T cast(const Variant &p_variant) {
		return p_variant;
	}
};

#define VARIANT_CAST_ENUM(m_enum)                                                                                     \
	template <>                                                                                                       \
	struct VariantCaster<m_enum> {                                                                                    \
		static FORCE_INLINE m_enum cast(const Variant &p_variant) {                                                   \
			return (m_enum)p_variant.operator int64_t();                                                              \
		}                                                                                                             \
	}
