#pragma once

#include <application/application.hpp>

namespace legion::rendering::detail
{
    constexpr const uint64 gl_mask = 0xFFFFFFFF;
    constexpr const uint64 lgn_mask = ~gl_mask;

# define LGN_VERTEX_SHADER 1
# define LGN_FRAGMENT_SHADER 2
# define LGN_GEOMETRY_SHADER 3
# define LGN_TESS_EVALUATION_SHADER 4
# define LGN_TESS_CONTROL_SHADER 5

    const uint64 gl_lgn_vertex_shader = GL_VERTEX_SHADER + (static_cast<uint64>(LGN_VERTEX_SHADER) << 32);
    const uint64 gl_lgn_fragment_shader = GL_FRAGMENT_SHADER + (static_cast<uint64>(LGN_FRAGMENT_SHADER) << 32);
    const uint64 gl_lgn_geometry_shader = GL_GEOMETRY_SHADER + (static_cast<uint64>(LGN_GEOMETRY_SHADER) << 32);
    const uint64 gl_lgn_tess_evaluation_shader = GL_TESS_EVALUATION_SHADER + (static_cast<uint64>(LGN_TESS_EVALUATION_SHADER) << 32);
    const uint64 gl_lgn_tess_control_shader = GL_TESS_CONTROL_SHADER + (static_cast<uint64>(LGN_TESS_CONTROL_SHADER) << 32);

# define GL_LGN_VERTEX_SHADER legion::rendering::detail::gl_lgn_vertex_shader
# define GL_LGN_FRAGMENT_SHADER legion::rendering::detail::gl_lgn_fragment_shader
# define GL_LGN_GEOMETRY_SHADER legion::rendering::detail::gl_lgn_geometry_shader
# define GL_LGN_TESS_EVALUATION_SHADER legion::rendering::detail::gl_lgn_tess_evaluation_shader
# define GL_LGN_TESS_CONTROL_SHADER legion::rendering::detail::gl_lgn_tess_control_shader

    inline unsigned long get_gl_type(unsigned long long v)
    {
        return static_cast<int>(v & gl_mask);
    }

    inline unsigned long get_lgn_type(unsigned long long v)
    {
        return static_cast<int>((v & lgn_mask) >> 32);
    }
}
