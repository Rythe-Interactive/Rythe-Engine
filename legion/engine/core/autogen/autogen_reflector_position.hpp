#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct position;
    template<>
    L_NODISCARD reflector make_reflector<position>(position& obj);
    template<>
    L_NODISCARD const reflector make_reflector<const position>(const position& obj);
}
