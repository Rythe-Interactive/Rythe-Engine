#pragma once
#include <core/types/prototype.hpp>
namespace legion::core
{
    struct window;
    template<>
    L_NODISCARD prototype make_prototype<window>(const window& obj);
}
