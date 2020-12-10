#pragma once
#include <application/application.hpp>
#include <rendering/data/buffer.hpp>

namespace legion::rendering
{
    struct vertexarray
    {
    private:
        common::managed_resource<app::gl_id> m_id = common::managed_resource<app::gl_id>(nullptr);

    public:
        vertexarray() = default;

        explicit vertexarray(std::nullptr_t t) : m_id([](app::gl_id& value) { glDeleteVertexArrays(1, &value); }, invalid_id)
        {
            glGenVertexArrays(1, &m_id);
        }

        static vertexarray generate()
        {
            return vertexarray(nullptr);
        }

        bool setAttribPointer(const buffer& buf, uint index, size_type size, GLenum type, bool normalized, size_type stride, size_type offset)
        {
            if (buf.target() != GL_ARRAY_BUFFER)
            {
                log::error("Attempted to set attribute pointer for non GL_ARRAY_BUFFER, attribute pointers only work for VBOs. VAO: {} Buffer: {}", m_id.value, buf.id());
                return false;
            }

            glBindVertexArray(m_id);
            buf.bind();
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<GLvoid*>(offset));
            buf.release();
            glBindVertexArray(0);
            return true;
        }

        void setAttribDivisor(uint index, uint divisor)
        {
            glBindVertexArray(m_id);
            glVertexAttribDivisor(index, divisor);
            glBindVertexArray(0);
        }

        void bind() const
        {
            glBindVertexArray(m_id);
        }

        static void release()
        {
            glBindVertexArray(0);
        }
    };
}
