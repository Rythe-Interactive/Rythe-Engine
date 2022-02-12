#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct example_comp;
    L_NODISCARD prototype make_prototype(example_comp& obj);
    L_NODISCARD prototype make_prototype(const example_comp& obj);
}
