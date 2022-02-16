#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct window;
    template<>
    L_NODISCARD reflector make_reflector<window>(window& obj);
    template<>
    L_NODISCARD const reflector make_reflector<const window>(const window& obj);
}
