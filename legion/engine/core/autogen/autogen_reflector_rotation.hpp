#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct rotation;
    template<>
    L_NODISCARD reflector make_reflector<rotation>(rotation& obj);
    template<>
    L_NODISCARD const reflector make_reflector<const rotation>(const rotation& obj);
}
