#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct rotation;
    template<>
    L_NODISCARD prototype make_prototype<rotation>(const rotation& obj);
}
