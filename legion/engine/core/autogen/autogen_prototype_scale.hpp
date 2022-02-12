#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct scale;
    L_NODISCARD prototype make_prototype(scale& obj);
    L_NODISCARD prototype make_prototype(const scale& obj);
}
