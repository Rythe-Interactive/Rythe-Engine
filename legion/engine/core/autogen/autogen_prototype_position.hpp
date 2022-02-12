#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct position;
    L_NODISCARD prototype make_prototype(position& obj);
    L_NODISCARD prototype make_prototype(const position& obj);
}
