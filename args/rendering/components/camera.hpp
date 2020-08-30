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

        void set_projection(float fovy, float ratio, float nearz)
        {
            float f = 1.0f / math::tan(fovy / 2.0f);
            projection = math::mat4(
                f / ratio, 0.0f, 0.0f, 0.0f,
                0.0f, f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, nearz, 0.0f);
        }
    };
}
