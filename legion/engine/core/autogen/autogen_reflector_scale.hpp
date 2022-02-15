#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct scale;
    template<>
    L_NODISCARD reflector make_reflector<scale>(scale& obj);
    template<>
    L_NODISCARD const reflector make_reflector<const scale>(const scale& obj);
}
