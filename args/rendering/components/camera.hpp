#pragma once
#include <rendering/detail/engine_include.hpp>

namespace args::rendering
{
    struct camera
    {
        float fov, nearz, farz;

        void set_projection(float fov, float nearz, float farz)
        {
            this->fov = fov;
            this->nearz = nearz;
            this->farz = farz;
        }

        math::mat4 get_projection(float ratio)
        {
           return math::perspective(math::deg2rad(fov * ratio), ratio, farz, nearz);
        }
    };
}
