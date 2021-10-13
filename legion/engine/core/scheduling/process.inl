#include <core/scheduling/process.hpp>
#pragma once

namespace legion::core::scheduling
{
    template<size_type charc>
    inline L_ALWAYS_INLINE Process::Process(const char(&name)[charc], time::span interval) : m_name(name), m_nameHash(nameHash<charc>(name))
    {
        setInterval(interval);
    }
}
