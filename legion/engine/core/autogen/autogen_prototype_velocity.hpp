#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct velocity;
    L_NODISCARD prototype make_prototype(velocity& obj);
    L_NODISCARD prototype make_prototype(const velocity& obj);
}
