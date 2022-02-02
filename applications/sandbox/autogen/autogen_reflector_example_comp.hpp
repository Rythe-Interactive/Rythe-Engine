#pragma once
#include <core/types/reflector.hpp>
#include "../systems\examplesystem.hpp"
struct example_comp;
namespace legion::core
{
    L_NODISCARD reflector make_reflector(example_comp& obj);
    L_NODISCARD reflector make_reflector(const example_comp& obj);
}
