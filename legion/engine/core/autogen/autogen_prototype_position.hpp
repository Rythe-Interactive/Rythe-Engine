#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct position;
    template<>
    L_NODISCARD prototype make_prototype<position>(const position& obj);
}
