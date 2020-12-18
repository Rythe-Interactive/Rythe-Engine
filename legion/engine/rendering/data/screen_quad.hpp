#pragma once
#include <rendering/data/buffer.hpp>
#include <rendering/data/vertexarray.hpp>

namespace legion::rendering
{
    struct screen_quad
    {
    public:
        screen_quad() = default;
        explicit screen_quad(std::nullptr_t)
        {
#if defined(LEGION_DEBUG)
            if (!app::ContextHelper::getCurrentContext())
            {
                log::error("No current context to create screen quad with.");
                return;
            }
#endif

            static float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
              // positions         // texCoords
              -1.0f,  1.0f,  1.0,  0.0f, 1.0f,
              -1.0f, -1.0f,  1.0,  0.0f, 0.0f,
               1.0f, -1.0f,  1.0,  1.0f, 0.0f,

              -1.0f,  1.0f,  1.0,  0.0f, 1.0f,
               1.0f, -1.0f,  1.0,  1.0f, 0.0f,
               1.0f,  1.0f,  1.0,  1.0f, 1.0f
            };

            m_quadVAO = vertexarray::generate();
            m_quadVBO = buffer(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            m_quadVAO.setAttribPointer(m_quadVBO, SV_POSITION, 3, GL_FLOAT, false, 5 * sizeof(float), 0);
            m_quadVAO.setAttribPointer(m_quadVBO, SV_TEXCOORD0, 2, GL_FLOAT, false, 5 * sizeof(float), 3 * sizeof(float));
        }

        static screen_quad generate()
        {
            return screen_quad(nullptr);
        }

        void render()
        {
#if defined(LEGION_DEBUG)
            if (!app::ContextHelper::getCurrentContext())
            {
                log::error("No current context to work with.");
                return;
            }
#endif

            m_quadVAO.bind();
            glDrawArrays(GL_TRIANGLES, 0, 6);
            m_quadVAO.release();
        }
    private:

        vertexarray m_quadVAO;
        buffer m_quadVBO;
    };
}
