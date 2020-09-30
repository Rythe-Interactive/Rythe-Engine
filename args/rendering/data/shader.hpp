#pragma once
#include <vector>
#include <string>
#include <rendering/data/model.hpp>
#include <rendering/data/texture.hpp>

/**
 * @file shader.hpp
 */

namespace args::rendering
{
    struct model;
    struct camera;
    struct shader;
    struct ShaderCache;
    struct shader_handle;

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

        shader_parameter_base(id_type shaderId, std::string_view name, GLenum type, GLint location) : m_shaderId(shaderId), m_name(name), m_type(type), m_location(location) {};

    public:
        /**@brief Returns whether the parameter is referencing a valid shader and parameter location.
         */
        virtual bool is_valid() const
        {
            return m_location != -1 && m_shaderId != invalid_id;
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

        /**@brief Set the value of the uniform.
         */
        void set_value(const T& value);
    };

    template<typename T>
    inline void uniform<T>::set_value(const T& value)
    {
    }

    template<>
    inline void uniform<texture>::set_value(const texture& value)
    {

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
        using shader_state = std::unordered_map<GLenum, GLenum>;
        GLint programId;
        std::unordered_map<id_type, std::unique_ptr<shader_parameter_base>> uniforms;
        std::unordered_map<id_type, std::unique_ptr<attribute>> attributes;
        std::string name;
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
            return *dynamic_cast<uniform<T>*>(uniforms[nameHash(name)].get());
        }

        template<typename T>
        uniform<T> get_uniform(id_type id)
        {
            return *dynamic_cast<uniform<T>*>(uniforms[id].get());
        }

        attribute get_attribute(const std::string& name)
        {
            id_type id = nameHash(name);
            if (attributes.count(id))
                return *(attributes[id].get());

            log::error("Shader {} does not contain attribute {}", this->name, name);
            return invalid_attribute;
        }

        attribute get_attribute(id_type id)
        {
            if (attributes.count(id))
                return *(attributes[id].get());
            log::error("Shader {} does not contain attribute with id {}", this->name, id);
            return invalid_attribute;
        }

        std::vector<std::pair<std::string, GLenum>> get_uniform_info()
        {
            std::vector<std::pair<std::string, GLenum>> info;
            for (auto& [_, uniform] : uniforms)
                info.push_back(std::make_pair(uniform->get_name(), uniform->get_type()));
            return std::move(info);
        }
    };

    struct ARGS_API shader_handle
    {
        using cache = ShaderCache;
        id_type id;
        GLuint get_uniform_block_index(const std::string& name) const;
        void bind_uniform_block(GLuint uniformBlockIndex, GLuint uniformBlockBinding) const;

        std::string get_name() const;

        std::vector<std::pair<std::string, GLenum>> get_uniform_info() const;

        template<typename T>
        uniform<T> get_uniform(const std::string& name)
        {
            return ShaderCache::get_shader(id)->get_uniform<T>(name);
        }

        template<typename T>
        uniform<T> get_uniform(id_type uniformId)
        {
            return ShaderCache::get_shader(id)->get_uniform<T>(uniformId);
        }

        attribute get_attribute(const std::string& name);

        attribute get_attribute(id_type attributeId);

        void bind();
        static void release();

        bool operator==(const shader_handle& other) const { return id == other.id; }
        operator bool() { return id != invalid_id; }
    };

    constexpr shader_handle invalid_shader_handle{ invalid_id };

    struct shader_import_settings
    {

    };

    constexpr shader_import_settings default_shader_settings{};

    class ARGS_API ShaderCache
    {
        friend class renderer;
        friend struct shader_handle;
    private:
        using shader_ilo = std::vector<std::pair<GLuint, std::string>>; // Shader intermediate language object.
        using shader_state = std::unordered_map<GLenum, GLenum>;

        static sparse_map<id_type, shader> m_shaders;
        static async::readonly_rw_spinlock m_shaderLock;

        static shader* get_shader(id_type id);

        static void process_includes(std::string& shaderSource);
        static void resolve_preprocess_features(std::string& shaderSource, shader_state& state);
        static shader_ilo seperate_shaders(std::string& shaderSource);
        static void process_io(shader& shader, id_type id);
        static app::gl_id compile_shader(GLuint shaderType, cstring source, GLint sourceLength);

    public:
        static shader_handle create_shader(const std::string& name, const fs::view& file, shader_import_settings settings = default_shader_settings);
        static shader_handle create_shader(const fs::view& file, shader_import_settings settings = default_shader_settings);
        static shader_handle get_handle(const std::string& name);
        static shader_handle get_handle(id_type id);
    };
}
