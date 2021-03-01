#include <core/common/hash.hpp>
#pragma once

namespace legion::core
{
    template<size_type N>
    inline constexpr name_hash::name_hash(const char(&name)[N]) noexcept
        : hash({ nameHash<N>(name) }) {}

    template<size_type N>
    inline constexpr name_hash::name_hash(const string_literal<N>& name) noexcept
        : hash({ nameHash<N>(name) }) {}

}
