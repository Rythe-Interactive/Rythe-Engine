#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct rotation;
    L_NODISCARD prototype make_prototype(rotation& obj);
    L_NODISCARD prototype make_prototype(const rotation& obj);
}
