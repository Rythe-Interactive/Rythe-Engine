#include <rendering/data/shader.hpp>

namespace args::rendering
{
    const shader& shader_cache::get_shader(id_type id)
    {
        async::readonly_guard guard(m_shaderLock);
        return m_shaders[id];
    }

    void shader_cache::process_includes(std::string& shaderSource)
    {
    }

    void shader_cache::process_mangling(std::string& shaderSource)
    {
    }

    shader_cache::shader_ilo shader_cache::seperate_shaders(std::string& shaderSource)
    {
        shader_ilo ilo;

        return ilo;
    }

    void shader_cache::process_io(shader& shader)
    {
    }

    app::gl_id shader_cache::compile_shader(GLuint shaderType, cstring source, GLint sourceLength)
    {
        app::gl_id shaderId = glCreateShader(shaderType);
        glShaderSource(shaderId, 1, &source, &sourceLength);
        glCompileShader(shaderId);

        GLint compilerResult = GL_FALSE;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compilerResult);

        if (!compilerResult)
        {
            GLint infoLogLength;
            glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
            char* errorMessage = new char[infoLogLength + 1];
            glGetShaderInfoLog(shaderId, infoLogLength, nullptr, errorMessage);

            cstring shaderTypename;
            switch (shaderType)
            {
            case GL_FRAGMENT_SHADER:
                shaderTypename = "fragment";
                break;
            case GL_VERTEX_SHADER:
                shaderTypename = "vertex";
                break;
            case GL_GEOMETRY_SHADER:
                shaderTypename = "geometry";
                break;
            case GL_TESS_CONTROL_SHADER:
                shaderTypename = "tessellation control";
                break;
            case GL_TESS_EVALUATION_SHADER:
                shaderTypename = "tessellation evaluation";
                break;
            case GL_COMPUTE_SHADER:
                shaderTypename = "compute";
                break;
            default:
                shaderTypename = "unknown type";
                break;
            }

            std::cout << "Error compiling " << shaderTypename << " shader:\n\t" << errorMessage << std::endl;
            delete[] errorMessage;

            glDeleteShader(shaderId);
            return -1;
        }
        return shaderId;
    }

    shader_handle shader_cache::create_shader(const std::string& name, const fs::view& file, shader_import_settings settings)
    {
        id_type id = nameHash(name);

        {
            async::readonly_guard guard(m_shaderLock);
            if (m_shaders.contains(id))
                return { id };
        }

        if (!file.is_valid() || !file.file_info().is_file)
            return invalid_shader_handle;

        auto result = file.get();

        if(result != common::valid)
            return invalid_shader_handle;

        std::string source = ((fs::basic_resource)result).to_string();

        process_includes(source);
        process_mangling(source);

        shader_ilo shaders = seperate_shaders(source);

        shader shader;
        shader.programId = glCreateProgram();

        std::vector<app::gl_id> shaderIds;

        for (auto& [shaderType, shaderIL] : shaders)
        {
            auto shaderId = compile_shader(shaderType, shaderIL.c_str(), shaderIL.size());

            if (shaderId < 0)
            {
                std::cout << "error occurred in shader: " << name.c_str() << std::endl;
                for (auto id : shaderIds)
                    glDeleteShader(id);

                glDeleteProgram(shader.programId);
                return invalid_shader_handle;
            }

            glAttachShader(shader.programId, shaderId);
            shaderIds.push_back(shaderId);
        }

        glLinkProgram(shader.programId);

        process_io(shader);

        {
            async::readwrite_guard guard(m_shaderLock);
            m_shaders.insert(id, shader);
        }

        return { id };
    }

}
