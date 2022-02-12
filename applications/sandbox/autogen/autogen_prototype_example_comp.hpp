#pragma once
#include <core/types/prototype.hpp>
#include "../../sandbox\systems\examplesystem.hpp"
struct example_comp;
namespace legion::core
{
    L_NODISCARD prototype make_prototype(example_comp& obj);
    L_NODISCARD prototype make_prototype(const example_comp& obj);
}
