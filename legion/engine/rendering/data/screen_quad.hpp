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

            static float quadVertices[24] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
              // positions   // texCoords
              -1.0f,  1.0f,  0.0f, 1.0f,
              -1.0f, -1.0f,  0.0f, 0.0f,
               1.0f, -1.0f,  1.0f, 0.0f,

              -1.0f,  1.0f,  0.0f, 1.0f,
               1.0f, -1.0f,  1.0f, 0.0f,
               1.0f,  1.0f,  1.0f, 1.0f
            };

            m_quadVAO = vertexarray::generate();
            m_quadVBO = buffer(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            m_quadVAO.setAttribPointer(m_quadVBO, 0, 2, GL_FLOAT, false, 4 * sizeof(float), 0);
            m_quadVAO.setAttribPointer(m_quadVBO, 1, 2, GL_FLOAT, false, 4 * sizeof(float), 2 * sizeof(float));
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
