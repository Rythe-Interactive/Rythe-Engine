# ifndef LGSL_PREPROCESSOR_DEFINED
# define LGSL_PREPROCESSOR_DEFINED
# if 0
GL_VERTEX_SHADER = 0x8B31
GL_FRAGMENT_SHADER = 0x8B30
GL_GEOMETRY_SHADER = 0x8DD9
GL_TESS_EVALUATION_SHADER = 0x8E87
GL_TESS_CONTROL_SHADER = 0x8E88
# else
#include <application/application.hpp>
#endif

# if 1 /*
""" */
namespace legion::rendering::detail
{
    constexpr const uint64
        /*"""
        # */
        gl_mask = 0xFFFFFFFF;
# endif

# if 1 /*
""" */
    constexpr const uint64
        /*"""
        # */
# endif
        lgn_mask = ~gl_mask;

# if 0

    LGN_VERTEX_SHADER = 1
        LGN_FRAGMENT_SHADER = 2
        LGN_GEOMETRY_SHADER = 3
        LGN_TESS_EVALUATION_SHADER = 4
        LGN_TESS_CONTROL_SHADER = 5

        GL_LGN_VERTEX_SHADER = GL_VERTEX_SHADER + (LGN_VERTEX_SHADER << 32)
        GL_LGN_FRAGMENT_SHADER = GL_FRAGMENT_SHADER + (LGN_FRAGMENT_SHADER << 32)
        GL_LGN_GEOMETRY_SHADER = GL_GEOMETRY_SHADER + (LGN_GEOMETRY_SHADER << 32)
        GL_LGN_TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER + (LGN_TESS_EVALUATION_SHADER << 32)
        GL_LGN_TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER + (LGN_TESS_CONTROL_SHADER << 32)
# else

# define LGN_VERTEX_SHADER 1
# define LGN_FRAGMENT_SHADER 2
# define LGN_GEOMETRY_SHADER 3
# define LGN_TESS_EVALUATION_SHADER 4
# define LGN_TESS_CONTROL_SHADER 5

# if 1 /*
""" */
    const uint64 gl_lgn_vertex_shader = GL_VERTEX_SHADER + (static_cast<uint64>(LGN_VERTEX_SHADER) << 32);
    const uint64 gl_lgn_fragment_shader = GL_FRAGMENT_SHADER + (static_cast<uint64>(LGN_FRAGMENT_SHADER) << 32);
    const uint64 gl_lgn_geometry_shader = GL_GEOMETRY_SHADER + (static_cast<uint64>(LGN_GEOMETRY_SHADER) << 32);
    const uint64 gl_lgn_tess_evaluation_shader = GL_TESS_EVALUATION_SHADER + (static_cast<uint64>(LGN_TESS_EVALUATION_SHADER) << 32);
    const uint64 gl_lgn_tess_control_shader = GL_TESS_CONTROL_SHADER + (static_cast<uint64>(LGN_TESS_CONTROL_SHADER) << 32);
    /*"""
    # */
# endif

# define GL_LGN_VERTEX_SHADER legion::rendering::detail::gl_lgn_vertex_shader
# define GL_LGN_FRAGMENT_SHADER legion::rendering::detail::gl_lgn_fragment_shader
# define GL_LGN_GEOMETRY_SHADER legion::rendering::detail::gl_lgn_geometry_shader
# define GL_LGN_TESS_EVALUATION_SHADER legion::rendering::detail::gl_lgn_tess_evaluation_shader
# define GL_LGN_TESS_CONTROL_SHADER legion::rendering::detail::gl_lgn_tess_control_shader
# endif
# if 0
    """
# endif

        inline unsigned long get_gl_type(unsigned long long v)
    {
        return static_cast<int>(v & gl_mask);
    }

    inline unsigned long get_lgn_type(unsigned long long v)
    {
        return static_cast<int>((v & lgn_mask) >> 32);
    }

}
# if 0
"""
# endif
# endif
