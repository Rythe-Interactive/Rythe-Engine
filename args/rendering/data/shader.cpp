#include <rendering/data/shader.hpp>

namespace args::rendering
{
    sparse_map<id_type, shader> ShaderCache::m_shaders;
    async::readonly_rw_spinlock ShaderCache::m_shaderLock;

    shader* ShaderCache::get_shader(id_type id)
    {
        async::readonly_guard guard(m_shaderLock);
        return &m_shaders[id];
    }

    void ShaderCache::replace_items(std::string& source, const std::string& item, const std::string& value)
    {
        size_type n = 0;
        while ((n = source.find(item, n)) != std::string::npos)
        {
            source.replace(n, item.size(), value);
            n += value.size();
        }
    }

    void ShaderCache::process_includes(std::string& shaderSource)
    {
    }

    void ShaderCache::resolve_preprocess_features(std::string& shaderSource, shader_state& state)
    {
        replace_items(shaderSource, "SV_POSITION", std::to_string(SV_POSITION));
        replace_items(shaderSource, "SV_MODELMATRIX", std::to_string(SV_MODELMATRIX));
        replace_items(shaderSource, "SV_VIEW", std::to_string(SV_VIEW));
        replace_items(shaderSource, "SV_PROJECT", std::to_string(SV_PROJECT));

        static std::unordered_map<std::string, GLenum> funcTypes;
        static bool funcTypesInitialized = false;
        if (!funcTypesInitialized)
        {
            funcTypesInitialized = true;
            funcTypes["DEPTH"] = GL_DEPTH_TEST;
            funcTypes["CULL"] = GL_CULL_FACE;
            //funcTypes["ALPHA"] = GL_BLEND;
            //funcTypes["DITHER"] = GL_DITHER;
        }
        state[GL_DEPTH_TEST] = GL_GREATER;
        state[GL_CULL_FACE] = GL_BACK;

        size_type n = 0;
        while ((n = shaderSource.find("#enable", n)) != std::string::npos)
        {
            size_type end = shaderSource.find('\n', n);

            auto tokens = common::split_string_at<' '>(shaderSource.substr(n, end - n));

            if (tokens.size() < 3)
                continue;

            if (!funcTypes.count(tokens[1]))
                continue;

            GLenum funcType = funcTypes.at(tokens[1]);
            GLenum param = GL_FALSE;
            switch (funcType)
            {
            case GL_DEPTH_TEST:
            {
                static std::unordered_map<std::string, GLenum> params;
                static bool initialized = false;
                if (!initialized)
                {
                    initialized = true;
                    params["NEVER"] = GL_NEVER;
                    params["LESS"] = GL_LESS;
                    params["EQUAL"] = GL_EQUAL;
                    params["LEQUAL"] = GL_LEQUAL;
                    params["GREATER"] = GL_GREATER;
                    params["NOTEQUAL"] = GL_NOTEQUAL;
                    params["GEQUAL"] = GL_GEQUAL;
                    params["ALWAYS"] = GL_ALWAYS;
                }

                param = params.at(tokens[2]);
            }
            break;
            case GL_CULL_FACE:
            {
                static std::unordered_map<std::string, GLenum> params;
                static bool initialized = false;
                if (!initialized)
                {
                    initialized = true;
                    params["FRONT"] = GL_FRONT;
                    params["BACK"] = GL_BACK;
                    params["FRONT_AND_BACK"] = GL_FRONT_AND_BACK;
                    params["OFF"] = GL_FALSE;
                }

                param = params.at(tokens[2]);
            }
            break;
            case GL_BLEND:
            {
                static std::unordered_map<std::string, GLenum> params;
                static bool initialized = false;
                if (!initialized)
                {
                    initialized = true;
                    params["ZERO"] = GL_ZERO;
                    params["ONE"] = GL_ONE;
                    params["SRC_COLOR"] = GL_SRC_COLOR;
                    params["ONE_MINUS_SRC_COLOR"] = GL_ONE_MINUS_SRC_COLOR;
                    params["DST_COLOR"] = GL_DST_COLOR;
                    params["ONE_MINUS_DST_COLOR"] = GL_ONE_MINUS_DST_COLOR;
                    params["SRC_ALPHA"] = GL_SRC_ALPHA;
                    params["ONE_MINUS_SRC_ALPHA"] = GL_ONE_MINUS_SRC_ALPHA;
                    params["DST_ALPHA"] = GL_DST_ALPHA;
                    params["ONE_MINUS_DST_ALPHA"] = GL_ONE_MINUS_DST_ALPHA;
                    params["CONSTANT_COLOR"] = GL_CONSTANT_COLOR;
                    params["ONE_MINUS_CONSTANT_COLOR"] = GL_ONE_MINUS_CONSTANT_COLOR;
                    params["CONSTANT_ALPHA"] = GL_CONSTANT_ALPHA;
                    params["ONE_MINUS_CONSTANT_ALPHA"] = GL_ONE_MINUS_CONSTANT_ALPHA;
                    params["SRC_ALPHA_SATURATE"] = GL_SRC_ALPHA_SATURATE;
                }
            }
            break;
            default:
                break;
            }

            state[funcType] = param;
            shaderSource.replace(n, end - n, "");
        }
    }

    ShaderCache::shader_ilo ShaderCache::seperate_shaders(std::string& shaderSource)
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

        ilo.push_back(std::make_pair<GLuint, std::string>(GL_VERTEX_SHADER, versionTxt + std::string(rest.data(), vertEnd).replace(vertOffset, 5, " main")));
        rest = std::string_view(rest.data() + vertEnd, rest.size() - vertEnd);

        auto geomOffset = rest.find(" geom(");
        if (geomOffset != std::string_view::npos)
        {
            auto geomEnd = geomOffset + std::string_view(rest.data() + geomOffset, rest.size() - geomOffset).find_first_of('}') + 1;

            ilo.push_back(std::make_pair<GLuint, std::string>(GL_GEOMETRY_SHADER, versionTxt + std::string(rest.data(), geomEnd).replace(geomOffset, 5, " main")));
            rest = std::string_view(rest.data() + geomEnd, rest.size() - geomEnd);
        }

        auto fragOffset = rest.find(" frag(");
        if (fragOffset == std::string_view::npos)
            return {};

        auto fragEnd = fragOffset + std::string_view(rest.data() + fragOffset, rest.size() - fragOffset).find_first_of('}') + 1;

        ilo.push_back(std::make_pair<GLuint, std::string>(GL_FRAGMENT_SHADER, versionTxt + std::string(rest.data(), fragEnd).replace(fragOffset, 5, " main")));
        rest = std::string_view(rest.data() + fragEnd, rest.size() - fragEnd);
        return ilo;
    }

    void ShaderCache::process_io(shader& shader, id_type id)
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
                uniform = new rendering::uniform<texture>(id, name, type, location);
            case GL_FLOAT:
                uniform = new rendering::uniform<float>(id, name, type, location);
                break;
            case GL_FLOAT_VEC2:
                uniform = new rendering::uniform<math::vec2>(id, name, type, location);
                break;
            case GL_FLOAT_VEC3:
                uniform = new rendering::uniform<math::vec3>(id, name, type, location);
                break;
            case GL_FLOAT_VEC4:
                uniform = new rendering::uniform<math::vec4>(id, name, type, location);
                break;
            case GL_INT:
                uniform = new rendering::uniform<int>(id, name, type, location);
                break;
            case GL_INT_VEC2:
                uniform = new rendering::uniform<math::ivec2>(id, name, type, location);
                break;
            case GL_INT_VEC3:
                uniform = new rendering::uniform<math::ivec3>(id, name, type, location);
                break;
            case GL_INT_VEC4:
                uniform = new rendering::uniform<math::ivec4>(id, name, type, location);
                break;
            case GL_BOOL:
                uniform = new rendering::uniform<bool>(id, name, type, location);
                break;
            case GL_BOOL_VEC2:
                uniform = new rendering::uniform<math::bvec2>(id, name, type, location);
                break;
            case GL_BOOL_VEC3:
                uniform = new rendering::uniform<math::bvec3>(id, name, type, location);
                break;
            case GL_BOOL_VEC4:
                uniform = new rendering::uniform<math::bvec4>(id, name, type, location);
                break;
            case GL_FLOAT_MAT2:
                uniform = new rendering::uniform<math::mat2>(id, name, type, location);
                break;
            case GL_FLOAT_MAT3:
                uniform = new rendering::uniform<math::mat3>(id, name, type, location);
                break;
            case GL_FLOAT_MAT4:
                uniform = new rendering::uniform<math::mat4>(id, name, type, location);
                break;
            default:
                continue;
            }

            shader.uniforms[nameHash(std::string(name).c_str())] = std::unique_ptr<shader_parameter_base>(uniform);
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

            shader.attributes[nameHash(std::string(name).c_str())] = std::unique_ptr<attribute>(new attribute(id, name, type, location));
        }

        delete[] attribNameBuffer;
    }

    app::gl_id ShaderCache::compile_shader(GLuint shaderType, cstring source, GLint sourceLength)
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

    shader_handle ShaderCache::create_shader(const std::string& name, const fs::view& file, shader_import_settings settings)
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
        shader_state state;

        process_includes(source);
        resolve_preprocess_features(source, state);

        shader_ilo shaders = seperate_shaders(source);
        if (shaders.empty())
            return invalid_shader_handle;

        shader shader;
        shader.state = state;

        for (auto& [func, param] : state)
        {
            if (param == GL_FALSE)
            {
                glDisable(func);
                continue;
            }

            glEnable(func);
            switch (func)
            {
            case GL_DEPTH_TEST:
            {
                glDepthFunc(param);
            }
            break;
            case GL_CULL_FACE:
            {
                glCullFace(param);
            }
            break;
            case GL_BLEND:
            {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            break;
            default:
                break;
            }
        }

        shader.programId = glCreateProgram();

        std::vector<app::gl_id> shaderIds;

        for (auto& [shaderType, shaderIL] : shaders)
        {
            auto shaderId = compile_shader(shaderType, shaderIL.c_str(), shaderIL.size());

            if (shaderId == (app::gl_id) - 1)
            {
                std::cout << "error occurred in shader: " << name.c_str() << std::endl;

                std::cout << shaderIL << std::endl;

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
            m_shaders.insert(id, std::move(shader));
        }

        return { id };
    }

    shader_handle ShaderCache::get_handle(const std::string& name)
    {
        id_type id = nameHash(name);

        async::readonly_guard guard(m_shaderLock);
        if (!m_shaders.contains(id))
            return invalid_shader_handle;
        else
            return { id };
    }

    shader_handle ShaderCache::get_handle(id_type id)
    {
        async::readonly_guard guard(m_shaderLock);
        if (!m_shaders.contains(id))
            return invalid_shader_handle;
        else
            return { id };
    }

    GLuint shader_handle::get_uniform_block_index(const std::string& name) const
    {
        return ShaderCache::get_shader(id)->get_uniform_block_index(name);
    }

    void shader_handle::bind_uniform_block(GLuint uniformBlockIndex, GLuint uniformBlockBinding) const
    {
        ShaderCache::get_shader(id)->bind_uniform_block(uniformBlockIndex, uniformBlockBinding);
    }

    std::string shader_handle::get_name() const
    {
        return ShaderCache::get_shader(id)->name;
    }

    std::vector<std::pair<std::string, GLenum>> shader_handle::get_uniform_info() const
    {
        return ShaderCache::get_shader(id)->get_uniform_info();
    }

    attribute shader_handle::get_attribute(const std::string& name)
    {
        return ShaderCache::get_shader(id)->get_attribute(name);
    }

    attribute shader_handle::get_attribute(id_type attributeId)
    {
        return ShaderCache::get_shader(id)->get_attribute(attributeId);
    }

    void shader_handle::bind()
    {
        ShaderCache::get_shader(id)->bind();
    }

    void shader_handle::release()
    {
        glUseProgram(0);
    }

    void shader::bind()
    {
        for (auto& [func, param] : state)
        {
            if (param == GL_FALSE)
            {
                glDisable(func);
                continue;
            }

            glEnable(func);
            switch (func)
            {
            case GL_DEPTH_TEST:
            {
                glDepthFunc(param);
            }
            break;
            case GL_CULL_FACE:
            {
                glCullFace(param);
            }
            break;
            case GL_BLEND:
            {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            break;
            default:
                break;
            }
        }

        glUseProgram(programId);
    }

    void shader::release()
    {
        glUseProgram(0);
    }

    GLuint shader::get_uniform_block_index(const std::string& name) const
    {
        return glGetUniformBlockIndex(programId, name.c_str());
    }

    void shader::bind_uniform_block(GLuint uniformBlockIndex, GLuint uniformBlockBinding) const
    {
        glUniformBlockBinding(programId, uniformBlockIndex, uniformBlockBinding);
    }

}
