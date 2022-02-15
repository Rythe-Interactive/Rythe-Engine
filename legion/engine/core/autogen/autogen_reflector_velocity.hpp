#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct velocity;
    template<>
    L_NODISCARD reflector make_reflector<velocity>(velocity& obj);
    template<>
    L_NODISCARD const reflector make_reflector<const velocity>(const velocity& obj);
}
