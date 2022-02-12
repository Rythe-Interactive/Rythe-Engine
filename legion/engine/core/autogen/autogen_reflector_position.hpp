#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct position;
    L_NODISCARD reflector make_reflector(position& obj);
    L_NODISCARD const reflector make_reflector(const position& obj);
}
