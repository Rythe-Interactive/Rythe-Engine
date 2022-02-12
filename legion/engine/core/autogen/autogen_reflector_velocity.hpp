#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct velocity;
    L_NODISCARD reflector make_reflector(velocity& obj);
    L_NODISCARD const reflector make_reflector(const velocity& obj);
}
