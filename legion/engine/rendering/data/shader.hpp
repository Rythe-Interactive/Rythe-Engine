#pragma once
#include <vector>
#include <string>
#include <rendering/data/model.hpp>
#include <rendering/data/texture.hpp>
#include <rendering/util/bindings.hpp>
#include <rendering/util/settings.hpp>

/**
 * @file shader.hpp
 */

namespace legion::rendering
{
    struct model;
    struct camera;
    struct shader;
    struct ShaderCache;
    struct shader_handle;

    using shader_ilo = std::vector<std::pair<GLuint, std::string>>; // Shader intermediate language object.
    using shader_state = std::unordered_map<GLenum, GLenum>;

#pragma region shader parameters
    /**@class shader_parameter_base
     * @brief Common base of all shader parameter types.
     */
    struct shader_parameter_base
    {
    protected:
        id_type m_shaderId;
        std::string m_name;
        GLenum m_type;
        GLint m_location;

        shader_parameter_base(std::nullptr_t t) : m_shaderId(invalid_id), m_name(""), m_type(0), m_location(-1) {};

        shader_parameter_base(id_type shaderId, std::string_view name, GLenum type, GLint location) : m_shaderId(shaderId), m_name(name), m_type(type), m_location(location) {};

    public:
        /**@brief Returns whether the parameter is referencing a valid shader and parameter location.
         */
        virtual bool is_valid() const
        {
            return m_location != -1;
        }

        /**@brief Returns the GLenum of the data type of the parameter.
         */
        virtual GLenum get_type() const { return m_type; }
        /**@brief Returns the name of the shader parameter.
         */
        virtual std::string get_name() const { return m_name; }
        /**@brief Returns the location of the shader parameter.
         */
        virtual GLint get_location() const { return m_location; }

        bool operator==(const shader_parameter_base& other)
        {
            return m_shaderId == other.m_shaderId && m_name == other.m_name && m_type == other.m_type && m_location == other.m_location;
        }

        bool operator!=(const shader_parameter_base& other)
        {
            return m_shaderId != other.m_shaderId || m_name != other.m_name || m_type != other.m_type || m_location != other.m_location;
        }
    };

    /**@class uniform
     * @brief Shader parameter that represents an uniform.
     */
    template<typename T>
    struct uniform : public shader_parameter_base
    {
    public:
        uniform(id_type shaderId, std::string_view name, GLenum type, GLint location) : shader_parameter_base(shaderId, name, type, location) {}
        uniform(std::nullptr_t t) : shader_parameter_base(t) {};
        /**@brief Set the value of the uniform.
         */
        void set_value(const T& value);
    };

    template<typename T>
    inline void uniform<T>::set_value(const T& value)
    {
    }

    template<>
    struct uniform<texture_handle> : public shader_parameter_base
    {
        uint m_textureUnit;
    public:
        uniform(id_type shaderId, std::string_view name, GLenum type, GLint location, uint textureUnit) : shader_parameter_base(shaderId, name, type, location), m_textureUnit(textureUnit) {}
        uniform(std::nullptr_t t) : shader_parameter_base(t) {};
        /**@brief Set the value of the uniform.
         */
        void set_value(const texture_handle& value)
        {
            texture tex;
            if (is_valid())
                tex = value.get_texture();
            else
                tex = invalid_texture_handle.get_texture();

            glActiveTexture(GL_TEXTURE0 + m_textureUnit);
            glBindTexture(GL_TEXTURE_2D, tex.textureId);
            glUniform1i(m_location, m_textureUnit);
            glActiveTexture(GL_TEXTURE0);
        }
    };

    template<>
    inline void uniform<uint>::set_value(const uint& value)
    {
        if (is_valid())
            glUniform1ui(m_location, value);
    }

    template<>
    inline void uniform<float>::set_value(const float& value)
    {
        if (is_valid())
            glUniform1f(m_location, value);
    }

    template<>
    inline void uniform<math::vec2>::set_value(const math::vec2& value)
    {
        if (is_valid())
            glUniform2fv(m_location, 1, math::value_ptr(value));
    }

    template<>
    inline void uniform<math::vec3>::set_value(const math::vec3& value)
    {
        if (is_valid())
            glUniform3fv(m_location, 1, math::value_ptr(value));
    }

    template<>
    inline void uniform<math::vec4>::set_value(const math::vec4& value)
    {
        if (is_valid())
            glUniform4fv(m_location, 1, math::value_ptr(value));
    }

    template<>
    inline void uniform<int>::set_value(const int& value)
    {
        if (is_valid())
            glUniform1i(m_location, value);
    }

    template<>
    inline void uniform<math::ivec2>::set_value(const math::ivec2& value)
    {
        if (is_valid())
            glUniform2iv(m_location, 1, math::value_ptr(value));
    }

    template<>
    inline void uniform<math::ivec3>::set_value(const math::ivec3& value)
    {
        if (is_valid())
            glUniform3iv(m_location, 1, math::value_ptr(value));
    }

    template<>
    inline void uniform<math::ivec4>::set_value(const math::ivec4& value)
    {
        if (is_valid())
            glUniform4iv(m_location, 1, math::value_ptr(value));
    }

    template<>
    inline void uniform<bool>::set_value(const bool& value)
    {
        if (is_valid())
            glUniform1i(m_location, value);
    }

    template<>
    inline void uniform<math::bvec2>::set_value(const math::bvec2& value)
    {
        if (is_valid())
            glUniform2iv(m_location, 1, math::value_ptr(math::ivec2(value)));
    }

    template<>
    inline void uniform<math::bvec3>::set_value(const math::bvec3& value)
    {
        if (is_valid())
            glUniform3iv(m_location, 1, math::value_ptr(math::ivec3(value)));
    }

    template<>
    inline void uniform<math::bvec4>::set_value(const math::bvec4& value)
    {
        if (is_valid())
            glUniform4iv(m_location, 1, math::value_ptr(math::ivec4(value)));
    }

    template<>
    inline void uniform<math::mat2>::set_value(const math::mat2& value)
    {
        if (is_valid())
            glUniformMatrix2fv(m_location, 1, false, math::value_ptr(value));
    }

    template<>
    inline void uniform<math::mat3>::set_value(const math::mat3& value)
    {
        if (is_valid())
            glUniformMatrix3fv(m_location, 1, false, math::value_ptr(value));
    }

    template<>
    inline void uniform<math::mat4>::set_value(const math::mat4& value)
    {
        if (is_valid())
            glUniformMatrix4fv(m_location, 1, false, math::value_ptr(value));
    }

    /**@class attribute
     * @brief Shader parameter that represents an attribute.
     */
    struct attribute : public shader_parameter_base
    {
    public:
        attribute(id_type shaderId, std::string_view name, GLenum type, GLint location) : shader_parameter_base(shaderId, name, type, location) {}

        /**@brief Attach the currently bound array buffer to this attribute.
         * @param size Number of components in the data tensor (1: r, 2: rg, 3: rgb, 4: rgba).
         * @param type Data type of the components in the tensor.
         * @param normalized Normalize the tensors before sending to VRAM.
         * @param stride Amount of bytes in-between valid data chunks.
         * @param pointer Amount of bytes until the first valid data chunk.
         */
        void set_attribute_pointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
        {
            glEnableVertexAttribArray(m_location);
            glVertexAttribPointer(m_location, size, type, normalized, stride, reinterpret_cast<const GLvoid*>(pointer));
        }

        /**@brief Disable the attribute after you're done with it. Leaving it enabled can cause performance issues due to limiting the GPU to move and reallocate it's VRAM.
         */
        void disable_attribute_pointer()
        {
            glDisableVertexAttribArray(m_location);
        }

        /**@brief Set an interval between the iteration increments of the array.
         * @param offset Offset for the targeting location (0: vector or matrix row0, 1: matrix row1, 2: matrix row2... etc.).
         * @param divisor Amount of instances between each iteration increment. 0 means the iteration will increment for each vertex.
         */
        void set_divisor(uint offset, uint divisor)
        {
            glVertexAttribDivisor(m_location + offset, divisor);
        }
    };

    /**@brief Invalid default attribute
     */
    const attribute invalid_attribute(0, std::string_view(), 0, 0);

#pragma endregion

    /**@class shader
     * @brief Abstraction class of a shader program.
     */
    struct shader
    {
        /**@brief Data-structure to hold mapping of context functions and parameters.
         */
        GLint programId;
        std::unordered_map<id_type, std::unique_ptr<shader_parameter_base>> uniforms;
        std::unordered_map<id_type, std::unique_ptr<attribute>> attributes;
        std::unordered_map<GLint, id_type> idOfLocation;
        std::string name;
        std::string path;
        shader_state state;

        // Since copying would mean that the in-vram version of the actual shader would also need to be copied, we don't allow copying.
        shader(const shader&) = delete;
        // Moving should be fine though.
        shader(shader&&) = default;
        shader() = default;

        shader& operator=(shader&&) = default;
        shader& operator=(const shader&) = delete;

        void bind();
        static void release();

        GLuint get_uniform_block_index(const std::string& name) const;
        void bind_uniform_block(GLuint uniformBlockIndex, GLuint uniformBlockBinding) const;

        template<typename T>
        uniform<T> get_uniform(const std::string& name)
        {
            OPTICK_EVENT();
            auto* ptr = dynamic_cast<uniform<T>*>(uniforms[nameHash(name)].get());
            if (ptr)
                return *ptr;
            log::error("Uniform of type {} does not exist with name {}.", typeName<T>(), name);
            return uniform<T>(nullptr);
        }

        template<typename T>
        bool has_uniform(const std::string& name)
        {
            OPTICK_EVENT();
            auto id = nameHash(name);
            return uniforms.count(id) && dynamic_cast<uniform<T>*>(uniforms[id].get()) != nullptr;
        }

        template<typename T>
        uniform<T> get_uniform(id_type id)
        {
            OPTICK_EVENT();
            auto* ptr = dynamic_cast<uniform<T>*>(uniforms[id].get());
            if (ptr)
                return *ptr;
            log::error("Uniform of type {} does not exist with id {}.", typeName<T>(), id);
            return uniform<T>(nullptr);
        }

        template<typename T>
        bool has_uniform(id_type id)
        {
            OPTICK_EVENT();
            return uniforms.count(id) && dynamic_cast<uniform<T>*>(uniforms[id].get()) != nullptr;
        }

        template<typename T>
        uniform<T> get_uniform_with_location(GLint location)
        {
            OPTICK_EVENT();
            auto* ptr = dynamic_cast<uniform<T>*>(uniforms[idOfLocation[location]].get());
            if (ptr)
                return *ptr;
            log::error("Uniform of type {} does not exist with location {}.", typeName<T>(), location);
            return uniform<T>(nullptr);
        }

        template<typename T>
        bool has_uniform_with_location(GLint location)
        {
            OPTICK_EVENT();
            return uniforms.count(idOfLocation[location]) && dynamic_cast<uniform<T>*>(uniforms[idOfLocation[location]].get()) != nullptr;
        }

        attribute get_attribute(const std::string& name)
        {
            OPTICK_EVENT();
            id_type id = nameHash(name);
            if (attributes.count(id))
                return *(attributes[id].get());

            log::error("Shader {} does not contain attribute {}", this->name, name);
            return invalid_attribute;
        }

        attribute get_attribute(id_type id)
        {
            OPTICK_EVENT();
            if (attributes.count(id))
                return *(attributes[id].get());
            log::error("Shader {} does not contain attribute with id {}", this->name, id);
            return invalid_attribute;
        }

        std::vector<std::tuple<std::string, GLint, GLenum>> get_uniform_info()
        {
            OPTICK_EVENT();
            std::vector<std::tuple<std::string, GLint, GLenum>> info;
            for (auto& [_, uniform] : uniforms)
                info.push_back(std::make_tuple(uniform->get_name(), uniform->get_location(), uniform->get_type()));
            return std::move(info);
        }
    };

    struct shader_handle
    {
        using cache = ShaderCache;
        id_type id;
        GLuint get_uniform_block_index(const std::string& name) const;
        void bind_uniform_block(GLuint uniformBlockIndex, GLuint uniformBlockBinding) const;

        std::string get_name() const;

        std::string get_path() const;

        std::vector<std::tuple<std::string, GLint, GLenum>> get_uniform_info() const;

        template<typename T>
        uniform<T> get_uniform(const std::string& name);

        template<typename T>
        bool has_uniform(const std::string& name);

        template<typename T>
        uniform<T> get_uniform(id_type uniformId);

        template<typename T>
        bool has_uniform(id_type uniformId);

        template<typename T>
        uniform<T> get_uniform_with_location(GLint location);

        template<typename T>
        bool has_uniform_with_location(GLint location);

        attribute get_attribute(const std::string& name);

        attribute get_attribute(id_type attributeId);

        void bind();
        static void release();

        bool operator==(const shader_handle& other) const { return id == other.id; }
        bool operator!=(const shader_handle& other) const { return id != other.id; }
        operator bool() const noexcept { return id != invalid_id; }

        template<typename Archive>
        void serialize(Archive& archive);
    };
    template<class Archive>
    void shader_handle::serialize(Archive& archive)
    {
        archive(id);
    }

    constexpr shader_handle invalid_shader_handle{ invalid_id };

    class ShaderCache
    {
        friend class renderer;
        friend struct shader_handle;
    private:

        static sparse_map<id_type, shader> m_shaders;
        static async::rw_spinlock m_shaderLock;

        static shader* get_shader(id_type id);

        static void process_io(shader& shader, id_type id);
        static app::gl_id compile_shader(GLuint shaderType, cstring source, GLint sourceLength);

        static bool load_precompiled(const fs::view& file, shader_ilo& ilo, shader_state& state);
        static void store_precompiled(const fs::view& file, const shader_ilo& ilo, const shader_state& state);

        static shader_handle create_invalid_shader(const fs::view& file, shader_import_settings settings = default_shader_settings);

    public:
        static shader_handle create_shader(const std::string& name, const fs::view& file, shader_import_settings settings = default_shader_settings);
        static shader_handle create_shader(const fs::view& file, shader_import_settings settings = default_shader_settings);
        static shader_handle get_handle(const std::string& name);
        static shader_handle get_handle(id_type id);
    };

    template<typename T>
    uniform<T> shader_handle::get_uniform(const std::string& name)
    {
        OPTICK_EVENT();
        return ShaderCache::get_shader(id)->get_uniform<T>(name);
    }

    template<typename T>
    inline bool shader_handle::has_uniform(const std::string& name)
    {
        OPTICK_EVENT();
        return ShaderCache::get_shader(id)->has_uniform<T>(name);
    }

    template<typename T>
    uniform<T> shader_handle::get_uniform(id_type uniformId)
    {
        OPTICK_EVENT();
        return ShaderCache::get_shader(id)->get_uniform<T>(uniformId);
    }

    template<typename T>
    inline bool shader_handle::has_uniform(id_type uniformId)
    {
        OPTICK_EVENT();
        return ShaderCache::get_shader(id)->has_uniform<T>(uniformId);
    }

    template<typename T>
    inline uniform<T> shader_handle::get_uniform_with_location(GLint location)
    {
        OPTICK_EVENT();
        return ShaderCache::get_shader(id)->get_uniform_with_location<T>(location);
    }

    template<typename T>
    inline bool shader_handle::has_uniform_with_location(GLint location)
    {
        OPTICK_EVENT();
        return ShaderCache::get_shader(id)->has_uniform_with_location<T>(location);
    }

}
