#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct rotation;
    L_NODISCARD reflector make_reflector(rotation& obj);
    L_NODISCARD const reflector make_reflector(const rotation& obj);
}
