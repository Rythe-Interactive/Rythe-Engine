#pragma once
#include <core/types/reflector.hpp>
namespace legion::core
{
    struct example_comp;
    L_NODISCARD reflector make_reflector(example_comp& obj);
    L_NODISCARD const reflector make_reflector(const example_comp& obj);
}
