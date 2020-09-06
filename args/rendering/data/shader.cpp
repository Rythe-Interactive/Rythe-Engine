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
        std::string_view rest(shaderSource.data(), shaderSource.size());
 
        auto versionOffset = rest.find("#version");
        std::string versionTxt;

        if (versionOffset != std::string::npos)
        {
            rest = std::string_view(rest.data() + versionOffset, rest.size() - versionOffset);
            auto versionEnd = rest.find_first_of('\n');
            versionTxt = std::string(rest.data(), versionEnd);
            rest = std::string_view(rest.data() + versionEnd, rest.size() - versionEnd);
        }

        auto vertOffset = rest.find(" vert(");
        if (vertOffset == std::string_view::npos)
            return {};

        auto vertEnd = vertOffset + std::string_view(rest.data() + vertOffset, rest.size() - vertOffset).find_first_of('}') + 1;

        ilo.push_back(std::make_pair(GL_VERTEX_SHADER, std::string(versionTxt) + std::string(rest.data(), vertEnd).replace(vertOffset, 5, " main")));
        rest = std::string_view(rest.data() + vertEnd, rest.size() - vertEnd);

        auto fragOffset = rest.find(" frag(");
        if (fragOffset == std::string_view::npos)
            return {};

        auto fragEnd = fragOffset + std::string_view(rest.data() + fragOffset, rest.size() - fragOffset).find_first_of('}') + 1;

        ilo.push_back(std::make_pair(GL_FRAGMENT_SHADER, std::string(versionTxt) + std::string(rest.data(), fragEnd).replace(fragOffset, 5, " main")));
        rest = std::string_view(rest.data() + fragEnd, rest.size() - fragEnd);
        return ilo;
    }

    void shader_cache::process_io(shader& shader, id_type id)
    {
        shader.uniforms.clear();
        shader.attributes.clear();

        GLint numActiveUniforms = 0;
        glGetProgramiv(shader.programId, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

        GLint maxUniformNameLength = 0;
        glGetProgramiv(shader.programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
        GLchar* uniformNameBuffer = new GLchar[maxUniformNameLength];

        for (int uniformId = 0; uniformId < numActiveUniforms; uniformId++)
        {
            GLint arraySize = 0; // use this later for uniform arrays.
            GLenum type = 0;
            GLsizei nameLength = 0;
            glGetActiveUniform(shader.programId, uniformId, (GLsizei)maxUniformNameLength, &nameLength, &arraySize, &type, uniformNameBuffer);

            std::string_view name(uniformNameBuffer, nameLength + 1);
            if (name.find('[') != std::string_view::npos)
                continue;

            app::gl_location location = glGetUniformLocation(shader.programId, uniformNameBuffer);

            shader_parameter_base* uniform = nullptr;

            switch (type)
            {
            case GL_SAMPLER_2D:
                uniform = new rendering::uniform<texture>({ id }, name, type, location);
            case GL_FLOAT:
                uniform = new rendering::uniform<float>({ id }, name, type, location);
                break;
            case GL_FLOAT_VEC2:
                uniform = new rendering::uniform<math::vec2>({ id }, name, type, location);
                break;
            case GL_FLOAT_VEC3:
                uniform = new rendering::uniform<math::vec3>({ id }, name, type, location);
                break;
            case GL_FLOAT_VEC4:
                uniform = new rendering::uniform<math::vec4>({ id }, name, type, location);
                break;
            case GL_INT:
                uniform = new rendering::uniform<int>({ id }, name, type, location);
                break;
            case GL_INT_VEC2:
                uniform = new rendering::uniform<math::ivec2>({ id }, name, type, location);
                break;
            case GL_INT_VEC3:
                uniform = new rendering::uniform<math::ivec3>({ id }, name, type, location);
                break;
            case GL_INT_VEC4:
                uniform = new rendering::uniform<math::ivec4>({ id }, name, type, location);
                break;
            case GL_BOOL:
                uniform = new rendering::uniform<bool>({ id }, name, type, location);
                break;
            case GL_BOOL_VEC2:
                uniform = new rendering::uniform<math::bvec2>({ id }, name, type, location);
                break;
            case GL_BOOL_VEC3:
                uniform = new rendering::uniform<math::bvec3>({ id }, name, type, location);
                break;
            case GL_BOOL_VEC4:
                uniform = new rendering::uniform<math::bvec4>({ id }, name, type, location);
                break;
            case GL_FLOAT_MAT2:
                uniform = new rendering::uniform<math::mat2>({ id }, name, type, location);
                break;
            case GL_FLOAT_MAT3:
                uniform = new rendering::uniform<math::mat3>({ id }, name, type, location);
                break;
            case GL_FLOAT_MAT4:
                uniform = new rendering::uniform<math::mat4>({ id }, name, type, location);
                break;
            default:
                continue;
            }

            shader.uniforms[nameHash(name)] = std::unique_ptr<shader_parameter_base>(uniform);
        }

        delete[] uniformNameBuffer;

        GLint numActiveAttribs = 0;
        glGetProgramiv(shader.programId, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);

        GLint maxAttribNameLength = 0;
        glGetProgramiv(shader.programId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
        GLchar* attribNameBuffer = new GLchar[maxAttribNameLength];

        for (int attrib = 0; attrib < numActiveAttribs; ++attrib)
        {
            GLint arraySize = 0; // use this later for attribute arrays.
            GLenum type = 0;
            GLsizei nameLength = 0;
            glGetActiveAttrib(shader.programId, attrib, (GLsizei)maxAttribNameLength, &nameLength, &arraySize, &type, attribNameBuffer);

            std::string_view name(attribNameBuffer, nameLength + 1);
            GLint location = glGetAttribLocation(shader.programId, attribNameBuffer);

            if (name.find('[') != std::string_view::npos)
                continue;

            shader.attributes[nameHash(name)] = std::unique_ptr<attribute>(new attribute({ id }, name, type, location));
        }

        delete[] attribNameBuffer;
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

        if (result != common::valid)
            return invalid_shader_handle;

        std::string source = ((fs::basic_resource)result).to_string();

        process_includes(source);
        process_mangling(source);

        shader_ilo shaders = seperate_shaders(source);
        if (shaders.empty())
            return invalid_shader_handle;

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

        process_io(shader, id);

        {
            async::readwrite_guard guard(m_shaderLock);
            m_shaders.insert(id, shader);
        }

        return { id };
    }

    shader_handle shader_cache::get_handle(const std::string& name)
    {
        id_type id = nameHash(name);

        async::readonly_guard guard(m_shaderLock);
        if (!m_shaders.contains(id))
            return invalid_shader_handle;
        else
            return { id };
    }

    shader_handle shader_cache::get_handle(id_type id)
    {
        async::readonly_guard guard(m_shaderLock);
        if (!m_shaders.contains(id))
            return invalid_shader_handle;
        else
            return { id };
    }

}
