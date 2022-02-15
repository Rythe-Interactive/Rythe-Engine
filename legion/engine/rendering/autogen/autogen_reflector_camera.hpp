#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct camera;
    template<>
    L_NODISCARD reflector make_reflector<camera>(camera& obj);
    template<>
    L_NODISCARD const reflector make_reflector<const camera>(const camera& obj);
}
