#pragma once
#include <vector>
#include <string>
#include <rendering/data/mesh.hpp>
#include <rendering/data/texture.hpp>

namespace args::rendering
{
	struct mesh;
	struct camera;
	struct shader;

    struct ARGS_API shader_handle
    {
        id_type id;
    };

    constexpr shader_handle invalid_shader_handle{ 0 };

#pragma region shader parameters
	class shader_parameter_base
	{
	protected:
        shader_handle m_shader;
		std::string m_name;
		GLenum m_type;
		GLint m_location;

		shader_parameter_base(shader_handle shader, std::string_view name, GLenum type, GLint location) : m_shader(shader), m_name(name), m_type(type), m_location(location) {};

	public:
		virtual bool is_valid() const
		{
			return m_location != -1 && m_shader.id != invalid_shader_handle.id;
		}

		virtual GLenum get_type() const { return m_type; }
		virtual GLint get_location() const { return m_location; }
	};

	template<typename T>
	struct uniform : public shader_parameter_base
	{
	public:
		uniform(shader_handle shader, std::string_view name, GLenum type, GLint location) : shader_parameter_base(shader, name, type, location) {}

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

	class attribute : public shader_parameter_base
	{
	public:
		attribute(shader_handle shader, std::string_view name, GLenum type, GLint location) : shader_parameter_base(shader, name, type, location) {}

		void set_attribute_pointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
		{
			glEnableVertexAttribArray(m_location);
			glVertexAttribPointer(m_location, size, type, normalized, stride, pointer);
		}

		void disable_attribute_pointer()
		{
			glDisableVertexAttribArray(m_location);
		}
	};

#pragma endregion

	struct shader
	{
		GLint programId;
		std::unordered_map<id_type, std::unique_ptr<shader_parameter_base>> uniforms;
		std::unordered_map<id_type, std::unique_ptr<attribute>> attributes;

		GLuint get_uniform_block_index(const std::string& pName) const;
		void bind_uniform_block(GLuint uniformBlockIndex, GLuint uniformBlockBinding) const;

		template<typename T>
		uniform<T>* get_uniform(const std::string& name)
		{
			return dynamic_cast<uniform<T>*>(uniforms[nameHash(name)].get());
		}

        attribute* get_attribute(const std::string& name)
        {
            return attributes[nameHash(name)].get();
        }
	};

	struct shader_import_settings
	{

	};

	constexpr shader_import_settings default_shader_settings{};

	struct ARGS_API shader_cache
	{
		friend class renderer;
		friend struct shader_handle;
	private:
		using shader_ilo = std::vector<std::tuple<GLuint, std::string>>; // Shader intermediate language object.

		static sparse_map<id_type, shader> m_shaders;
		static async::readonly_rw_spinlock m_shaderLock;

		static const shader& get_shader(id_type id);

		static void process_includes(std::string& shaderSource);
		static void process_mangling(std::string& shaderSource);
		static shader_ilo seperate_shaders(std::string& shaderSource);
		static void process_io(shader& shader, id_type id);
		static app::gl_id compile_shader(GLuint shaderType, cstring source, GLint sourceLength);

	public:
		static shader_handle create_shader(const std::string& name, const fs::view& file, shader_import_settings settings = default_shader_settings);
		static shader_handle get_handle(const std::string& name);
		static shader_handle get_handle(id_type id);
	};
}
