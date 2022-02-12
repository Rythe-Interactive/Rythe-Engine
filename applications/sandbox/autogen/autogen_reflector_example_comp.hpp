#pragma once
#include <core/types/reflector.hpp>
#include "../../sandbox\systems\examplesystem.hpp"
struct example_comp;
namespace legion::core
{
    L_NODISCARD auto make_reflector(example_comp& obj)->std::conditional_t<std::is_const_v<example_comp>, const reflector, reflector>;
    L_NODISCARD const auto make_reflector(const example_comp& obj);
}
