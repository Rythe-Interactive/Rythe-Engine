#pragma once
#include <application/application.hpp>

/**
 * @file buffer.hpp
 */

namespace legion::rendering
{
    /**@class buffer
     * @brief Low level rendering buffer that can be used for all kinds of purposes.
     *        Depending on the target it could be a vertex buffer or shader storage buffer or something else.
     */
    struct buffer
    {
    private:
        // Managed resource with the buffer id, also has the responsibility of deleting the buffer after all copies of this buffer have been destroyed.
        common::managed_resource<app::gl_id> m_id = common::managed_resource<app::gl_id>(nullptr);
        GLenum m_target = invalid_id; // Buffer target
        GLenum m_usage = invalid_id; // Buffer intended memory usage.
                                     // This can be changed by OpenGL if the driver thinks you made the wrong choice,
                                     // in that case a warning will show in the console and this value will not be accurate anymore.

        static void idDeleter(app::gl_id& value);

    public:
        /**@brief Faux constructor. To prevent unnecessary GPU allocations the default constructor doesn't actually create a buffer.
         *        This means that default initialized buffers are invalid temporary objects until they get properly initialized.
         */
        buffer() = default;

        /**@brief Main allocating constructor. This constructor actually creates the GPU side buffer and binds it to this instance.
         * @note Read more at <a href="http://docs.gl/gl4/glBufferData">docs.gl.</a>
         * @param target The buffer type to create. eg: GL_ARRAY_BUFFER, GL_SHADER_STORAGE_BUFFER
         * @param data Vector of data to store in the buffers VRAM allocation.
         * @param usage Intended memory usage. eg: GL_STATIC_DRAW (Data doesn't change and ised for drawing many times), GL_DYNAMIC_READ (Changes all the time and is used for reading back to CPU)
         */
        template<typename T>
        buffer(GLenum target, const std::vector<T>& data, GLenum usage);

        /**@brief Main allocating constructor. This constructor actually creates the GPU side buffer and binds it to this instance.
         * @note Read more at <a href="http://docs.gl/gl4/glBufferData">docs.gl.</a>
         * @param target The buffer type to create. eg: GL_ARRAY_BUFFER, GL_SHADER_STORAGE_BUFFER
         * @param size Size in bytes to allocate on VRAM.
         * @param data Data to send to VRAM. This may be a nullptr if you only want to allocate VRAM but keep the memory empty.
         * @param usage Intended memory usage. eg: GL_STATIC_DRAW (Data doesn't change and ised for drawing many times), GL_DYNAMIC_READ (Changes all the time and is used for reading back to CPU)
         * @note If the buffer is allocated with a nullptr as the data pointer then the memory in VRAM may be garbage when read. Do not assume null writes to happen.
         */
        buffer(GLenum target, size_type size, void* data, GLenum usage);

        /**@brief Main non allocating constructor. This constructor actually creates the GPU side buffer and binds it to this instance.
         * @param target The buffer type to create. eg: GL_ARRAY_BUFFER, GL_SHADER_STORAGE_BUFFER
         * @param usage Intended memory usage. eg: GL_STATIC_DRAW (Data doesn't change and ised for drawing many times), GL_DYNAMIC_READ (Changes all the time and is used for reading back to CPU)
         * @note This constructor doesn't actually allocate any VRAM to this buffer yet and requires a bufferData call in order to allocate memory for it.
         */
        buffer(GLenum target, GLenum usage);

        /**@brief Returns the rendering API id of the buffer. Useful for low level native rendering.
         */
        L_NODISCARD app::gl_id id() const;

        /**@brief Returns the rendering API type/target of the buffer. Useful for low level native rendering.
         */
        L_NODISCARD GLenum target() const;

        /**@brief Returns the rendering API intended memory usage of the buffer. Useful for low level native rendering.
         */
        L_NODISCARD GLenum usage() const;

        /**@brief Returns the current size of the buffer in bytes.
         */
        L_NODISCARD size_type size() const;

        /**@brief Bind buffer to a set indexed buffer binding location in shaders.
         * @note Read more at <a href="http://docs.gl/gl4/glBindBufferBase">docs.gl.</a>
         * @param index Indexed buffer binding location to bind to.
         * @note Target must be GL_ATOMIC_COUNTER_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER or GL_SHADER_STORAGE_BUFFER.
         */
        void bindBufferBase(uint index) const;

        /**@brief Resize the buffer to a new size. This reallocates VRAM and thus invalidates all data in the buffer.
         */
        void resize(size_type newSize) const;

        /**@brief Send new data to the VRAM of the buffer. This may reallocate the buffer if the size of the new data is larger than the previously allocated VRAM.
         * @param data Vector with the data to send.
         */
        template<typename T>
        void bufferData(const std::vector<T>& data) const;

        /**@brief Send new data to the VRAM of the buffer. This may reallocate the buffer if the size of the new data is larger than the previously allocated VRAM.
         * @param size Size the data to send.
         * @param data Pointer to the data to send. (Should not be a nullptr, if you only want to allocate/reallocate then use the resize function.)
         */
        void bufferData(size_type size, void* data) const;

        /**@brief Send new data to the VRAM of the buffer with a certain offset. This function does not reallocate data and thus the sum of offset + size cannot be more that the size of the buffer.
         * @param offset Offset since the start of the buffer in bytes.
         * @param size Size of the new data in bytes.
         * @param data Pointer to the new data to write.
         */
        void bufferData(size_type offset, size_type size, void* data) const;

        /**@brief Bind the buffer to the current context. Useful for low level native rendering.
         */
        void bind() const;

        /**@brief Release the buffer from the current context. Useful for low level native rendering.
         */
        void release() const;
    };
}

#include <rendering/data/buffer.inl>
