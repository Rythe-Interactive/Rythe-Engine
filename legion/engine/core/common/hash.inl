#include <core/common/hash.hpp>
#pragma once

namespace legion::core
{
    constexpr name_hash::operator id_type() const noexcept
    {
        return value;
    }

    constexpr id_type combine_hash(id_type seed, id_type value)
    {
        value += 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed ^ value;
    }

    constexpr name_hash::name_hash() noexcept {}

    constexpr name_hash::name_hash(id_type val) noexcept
        : value(val) {}

    template<size_type N>
    inline constexpr name_hash::name_hash(const char(&name)[N]) noexcept
        : value(nameHash<N>(name)) {}

    template<size_type N>
    inline constexpr name_hash::name_hash(const string_literal<N>& name) noexcept
        : value(nameHash<N>(name)) {}

    constexpr name_hash literals::operator""_hs(cstring src)
    {
        return name_hash{ nameHash(src) };
    }

}
