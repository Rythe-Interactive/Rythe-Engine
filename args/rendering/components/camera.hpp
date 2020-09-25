#pragma once
#include <rendering/detail/engine_include.hpp>

namespace args::rendering
{
    struct camera
    {
        math::mat4 projection = math::mat4(
            0.974279f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.732051f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 0.1f, 0.0f);

        void set_projection(float fov, float ratio, float nearz, float farz)
        {
            projection = math::perspective(math::deg2rad(fov * ratio), ratio, farz, nearz);
        }
    };
}
