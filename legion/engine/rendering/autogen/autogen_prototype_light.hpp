#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct light;
    template<>
    L_NODISCARD prototype make_prototype<light>(const light& obj);
}
