#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct velocity;
    template<>
    L_NODISCARD prototype make_prototype<velocity>(const velocity& obj);
}
