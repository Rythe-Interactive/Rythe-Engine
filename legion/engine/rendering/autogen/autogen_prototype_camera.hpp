#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct camera;
    template<>
    L_NODISCARD prototype make_prototype<camera>(const camera& obj);
}
