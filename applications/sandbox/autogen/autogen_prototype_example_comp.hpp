#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct example_comp;
    template<>
    L_NODISCARD prototype make_prototype<example_comp>(const example_comp& obj);
}
