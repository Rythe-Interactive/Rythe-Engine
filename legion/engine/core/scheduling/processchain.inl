#include <core/scheduling/processchain.hpp>
#pragma once

namespace legion::core::scheduling
{
    template<size_type charc>
    inline L_ALWAYS_INLINE ProcessChain::ProcessChain(const char(&name)[charc]) : m_name(name), m_nameHash(nameHash<charc>(name))
    {
    }
}
