#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct scale;
    template<>
    L_NODISCARD prototype make_prototype<scale>(const scale& obj);
}