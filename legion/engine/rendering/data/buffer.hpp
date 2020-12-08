#pragma once
#include <application/application.hpp>

namespace legion::rendering
{
    struct buffer
    {
    private:
        common::managed_resource<app::gl_id> m_id = common::managed_resource<app::gl_id>(nullptr);
        GLenum m_target = invalid_id;
        GLenum m_usage = invalid_id;

    public:
        buffer() = default;

        template<typename T>
        buffer(GLenum target, const std::vector<T>& data, GLenum usage)
            : m_id([](app::gl_id& value) { glDeleteBuffers(1, &value); }, invalid_id), m_target(target), m_usage(usage)
        {
            glGenBuffers(1, &m_id);
            glBindBuffer(target, m_id);
            glBufferData(target, sizeof(T) * data.size(), data.data(), usage);
            glBindBuffer(target, 0);
        }

        buffer(GLenum target, size_type size, void* data, GLenum usage)
            : m_id([](app::gl_id& value) { glDeleteBuffers(1, &value); }, invalid_id), m_target(target), m_usage(usage)
        {
            glGenBuffers(1, &m_id);
            glBindBuffer(target, m_id);
            glBufferData(target, size, data, usage);
            glBindBuffer(target, 0);
        }

        L_NODISCARD app::gl_id id() const
        {
            return m_id;
        }

        L_NODISCARD GLenum target() const
        {
            return m_target;
        }

        L_NODISCARD GLenum usage() const
        {
            return m_usage;
        }

        L_NODISCARD size_type size() const
        {
            size_type size;
            glBindBuffer(m_target, m_id);
            glGetBufferParameteri64v(m_target, GL_BUFFER_SIZE, reinterpret_cast<GLint64*>(&size));
            glBindBuffer(m_target, 0);
            return size;
        }

        bool bindBufferBase(uint index)
        {
            if (m_target != GL_ATOMIC_COUNTER_BUFFER && m_target != GL_TRANSFORM_FEEDBACK_BUFFER && m_target != GL_UNIFORM_BUFFER && m_target != GL_SHADER_STORAGE_BUFFER)
            {
                log::error("Attempt at binding buffer base of an invalid target. Target must be GL_ATOMIC_COUNTER_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER or GL_SHADER_STORAGE_BUFFER. id: {}", m_id.value);
                return false;
            }

            glBindBuffer(m_target, m_id);
            glBindBufferBase(m_target, index, m_id);
            glBindBuffer(m_target, 0);
            return true;
        }

        void resize(size_type newSize) const
        {
            glBindBuffer(m_target, m_id);
            glBufferData(m_target, newSize, nullptr, m_usage);
            glBindBuffer(m_target, 0);
        }

        template<typename T>
        void bufferData(const std::vector<T>& data)
        {
            glBindBuffer(m_target, m_id);

            auto oldSize = size();
            auto dataSize = data.size() * sizeof(T);
            if(oldSize >= dataSize)
                glBufferSubData(m_target, 0, dataSize, data.data());
            else
                glBufferData(m_target, dataSize, data.data(), m_usage);

            glBindBuffer(m_target, 0);
        }

        void bufferData(size_type size, void* data) const
        {
            glBindBuffer(m_target, m_id);
            glBufferData(m_target, size, data, m_usage);
            glBindBuffer(m_target, 0);
        }

        void bufferData(size_type offset, size_type size, void* data) const
        {
            glBindBuffer(m_target, m_id);
            glBufferSubData(m_target, offset, size, data);
            glBindBuffer(m_target, 0);
        }

        void bind() const
        {
            glBindBuffer(m_target, m_id);
        }

        void release() const
        {
            glBindBuffer(m_target, 0);
        }
    };
}
