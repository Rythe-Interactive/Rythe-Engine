#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct light;
    template<>
    L_NODISCARD reflector make_reflector<light>(light& obj);
    template<>
    L_NODISCARD const reflector make_reflector<const light>(const light& obj);
}
