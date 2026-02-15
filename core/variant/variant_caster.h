#pragma once

#include "core/typedefs.h"

#include "variant.h"

template<typename T>
struct VariantCaster {
    static FORCE_INLINE T cast(const Variant &p_variant) {
        return p_variant;
    }
};

/**
 * @brief Macro to create a caster that converts a Variant integer into an enum value. Useful for function pointers that require `Variants` being
 * converted from their `Variant` value into a C++-specific value.
 * @param m_enum The enum to create a converter for.
 */
#define VARIANT_CAST_ENUM(m_enum)                                           \
    template<>                                                              \
    struct VariantCaster<m_enum> {                                          \
        static FORCE_INLINE m_enum cast(const Variant &p_variant) {         \
            return (m_enum)p_variant.operator i64();                        \
        }                                                                   \
    };