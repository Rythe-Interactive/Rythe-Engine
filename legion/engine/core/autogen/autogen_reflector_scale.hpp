#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct scale;
    L_NODISCARD reflector make_reflector(scale& obj);
    L_NODISCARD const reflector make_reflector(const scale& obj);
}
