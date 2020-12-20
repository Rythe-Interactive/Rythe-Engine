#include <rendering/shadercompiler/shadercompiler.hpp>
#include <rendering/util/settings.hpp>
#include <lgnspre/gl_consts.hpp>
#include <application/application.hpp>

namespace legion::rendering
{
    delegate<void(const std::string&, log::severity)> ShaderCompiler::m_callback;


    std::string ShaderCompiler::get_view_path(const fs::view& view, bool mustBeFile)
    {
        OPTICK_EVENT();
        using severity = log::severity;

        fs::navigator navigator(view.get_virtual_path());
        auto solution = navigator.find_solution();
        if (solution.has_err())
        {
            m_callback(std::string("Shader processor error: ") + solution.get_error().what(), severity::error);
            return "";
        }

        auto s = solution.get();
        if (s.size() != 1)
        {
            m_callback("Shader processor error: invalid file, fs::view was not fully local", severity::error);
            return "";
        }

        fs::basic_resolver* resolver = dynamic_cast<fs::basic_resolver*>(s[0].first);
        if (!resolver)
        {
            m_callback("Shader processor error: invalid file, fs::view was not local", severity::error);
            return "";
        }

        resolver->set_target(s[0].second);

        if (!resolver->is_valid())
        {
            m_callback("Shader processor error: invalid path", severity::error);
            return "";
        }

        if (mustBeFile && !resolver->is_file())
        {
            m_callback("Shader processor error: not a file", severity::error);
            return "";
        }

        return resolver->get_absolute_path();
    }

    const std::string& ShaderCompiler::get_shaderlib_path()
    {
        OPTICK_EVENT();
        static std::string libPath;
        if (libPath.empty())
            libPath = get_view_path(fs::view("engine://shaderlib"), false);
        return libPath;
    }

    const std::string& ShaderCompiler::get_compiler_path()
    {
        OPTICK_EVENT();
        static std::string compPath;
        if (compPath.empty())
            compPath = get_view_path(fs::view("engine://tools"), false) + fs::strpath_manip::separator() + "lgnspre";
        return compPath;
    }

    void ShaderCompiler::extract_state(std::string_view source, shader_state& state)
    {
        OPTICK_EVENT();
        std::string_view rest = source;
        std::vector<std::pair<std::string, std::string>> stateInput;
        while (!rest.empty())
        {
            auto seperator = rest.find_first_not_of('\n');
            seperator = rest.find_first_of('\n', seperator) + 1;

            if (seperator == 0)
                seperator = rest.size();

            auto line = rest.substr(0, seperator);
            auto space = line.find(' ');
            if (space != std::string::npos)
                stateInput.push_back(std::make_pair(std::string(line.substr(0, space)), std::string(line.substr(space + 1))));
            else
                stateInput.push_back(std::make_pair(std::string(line.substr(0, space)), std::string()));

            rest = rest.substr(seperator);
            seperator = rest.find_first_not_of('\n');
            if (seperator == std::string::npos)
                break;
        }

        // Create lookup table for the OpenGL function types that can be changed by the shader state.
        static std::unordered_map<std::string, GLenum> funcTypes;
        static bool funcTypesInitialized = false;
        if (!funcTypesInitialized)
        {
            funcTypesInitialized = true;
            funcTypes["DEPTH"] = GL_DEPTH_TEST;
            funcTypes["CULL"] = GL_CULL_FACE;
            funcTypes["ALPHA_SOURCE"] = GL_BLEND_SRC;
            funcTypes["ALPHA_DEST"] = GL_BLEND_DST;
            funcTypes["ALPHA"] = GL_BLEND;
            funcTypes["DITHER"] = GL_DITHER;
        }

        // Default shader state in case nothing was specified by the shader.
        state[GL_DEPTH_TEST] = GL_GREATER;
        state[GL_CULL_FACE] = GL_BACK;
        state[GL_BLEND_SRC] = GL_SRC_ALPHA;
        state[GL_BLEND_DST] = GL_ONE_MINUS_SRC_ALPHA;
        state[GL_DITHER] = GL_FALSE;

        for (auto& [func, par] : stateInput)
        {
            if (!funcTypes.count(func)) // If the function type is unsupported or unknown we also want to fail compilation.
                continue;

            GLenum funcType = funcTypes.at(func); // Fetch the function type without risking editing the lookup table.
            GLenum param = GL_FALSE;

            switch (funcType)
            {
            case GL_DEPTH_TEST:
            {
                static std::unordered_map<std::string, GLenum> params; // Initialize parameter lookup table.
                static bool initialized = false;
                if (!initialized)
                {
                    initialized = true;
                    params["OFF"] = GL_FALSE;
                    params["NEVER"] = GL_NEVER;
                    params["LESS"] = GL_LESS;
                    params["EQUAL"] = GL_EQUAL;
                    params["LEQUAL"] = GL_LEQUAL;
                    params["GREATER"] = GL_GREATER;
                    params["NOTEQUAL"] = GL_NOTEQUAL;
                    params["GEQUAL"] = GL_GEQUAL;
                    params["ALWAYS"] = GL_ALWAYS;
                }

                if (!params.count(par))
                    continue;

                param = params.at(par);
            }
            break;
            case GL_CULL_FACE:
            {
                static std::unordered_map<std::string, GLenum> params; // Initialize parameter lookup table.
                static bool initialized = false;
                if (!initialized)
                {
                    initialized = true;
                    params["FRONT"] = GL_FRONT;
                    params["BACK"] = GL_BACK;
                    params["FRONT_AND_BACK"] = GL_FRONT_AND_BACK;
                    params["OFF"] = GL_FALSE;
                }

                if (!params.count(par))
                    continue;

                param = params.at(par);
            }
            break;
            case GL_BLEND:
            case GL_BLEND_SRC:
            case GL_BLEND_DST:
            {
                static std::unordered_map<std::string, GLenum> params; // Initialize parameter lookup table.
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
                    params["SRC_ALPHL_SATURATE"] = GL_SRC_ALPHL_SATURATE;
                    params["OFF"] = GL_FALSE;
                }

                if (!params.count(par))
                    continue;

                param = params.at(par);
            }
            break;
            case GL_DITHER:
            {
                static std::unordered_map<std::string, GLenum> params; // Initialize parameter lookup table.
                static bool initialized = false;
                if (!initialized)
                {
                    initialized = true;
                    params["OFF"] = GL_FALSE;
                    params["ON"] = GL_TRUE;
                    params["FALSE"] = GL_FALSE;
                    params["TRUE"] = GL_TRUE;
                }

                if (!params.count(par))
                    continue;

                param = params.at(par);
            }
            break;
            default:
                param = GL_FALSE;
                break;
            }

            state[funcType] = param;
        }
    }

    bool ShaderCompiler::extract_ilo(std::string_view source, uint64 shaderType, shader_ilo& ilo)
    {
        OPTICK_EVENT();
        using severity = log::severity;

        GLuint glShaderType = detail::get_gl_type(shaderType);
        auto lgnShaderType = detail::get_lgn_type(shaderType);

        if (shaderType != GL_LGN_VERTEX_SHADER &&
            shaderType != GL_LGN_FRAGMENT_SHADER &&
            shaderType != GL_LGN_GEOMETRY_SHADER &&
            shaderType != GL_LGN_TESS_EVALUATION_SHADER &&
            shaderType != GL_LGN_TESS_CONTROL_SHADER)
        {
            switch (lgnShaderType)
            {
            case LGN_VERTEX_SHADER:
                glShaderType = GL_VERTEX_SHADER;
                break;
            case LGN_FRAGMENT_SHADER:
                glShaderType = GL_FRAGMENT_SHADER;
                break;
            case LGN_GEOMETRY_SHADER:
                glShaderType = GL_GEOMETRY_SHADER;
                break;
            case LGN_TESS_EVALUATION_SHADER:
                glShaderType = GL_TESS_EVALUATION_SHADER;
                break;
            case LGN_TESS_CONTROL_SHADER:
                glShaderType = GL_TESS_CONTROL_SHADER;
                break;
            default:
                m_callback("Shader processor error: unkown shader type", severity::error);
                return false;
                break;
            }
        }

        auto shadercode = std::string(source);
        ilo.push_back(std::make_pair(glShaderType, shadercode));
        return true;
    }

    std::string ShaderCompiler::invoke_compiler(const fs::view& file, bitfield8 compilerSettings, const std::vector<std::string>& defines, const std::vector<std::string>& additionalIncludes)
    {
        OPTICK_EVENT();
        using severity = log::severity;

        std::string filepath = get_view_path(file, true);
        if (filepath.empty())
            return "";

        auto folderEnd = filepath.find_last_of("\\/");
        std::string folderPath(filepath.c_str(), folderEnd);

        std::string definesString = " -D LEGION";
        if (compilerSettings & shader_compiler_options::debug)
            definesString += " -D DEBUG";
        else
            definesString += " -D RELEASE";

        if (compilerSettings & shader_compiler_options::low_power)
            definesString += " -D LOW_POWER";
        else
            definesString += " -D HIGH_PERFORMANCE";

        bitfield8 apiSet = 0;
        if (compilerSettings & shader_compiler_options::api_opengl)
        {
            apiSet |= rendering_api::opengl;
            definesString += " -D API_OPENGL";
        }

        if (compilerSettings & shader_compiler_options::api_vulkan)
        {
            if (apiSet)
            {
                if (apiSet & rendering_api::opengl)
                    m_callback("Shader processor warning: both OpenGL and Vulkan were set as graphics api, OpenGL is assumed", severity::warn);
            }
            else
            {
                apiSet |= rendering_api::vulkan;
                definesString += " -D API_VULKAN";
            }
        }

        if (!apiSet)
        {
            m_callback("Shader processor warning: no api was set, OpenGL is assumed", severity::warn);
            definesString += " -D API_UNKNOWN -D API_OPENGL";
        }

        for (auto def : defines)
        {
            definesString += " -D " + def;
        }

        std::string includeString = " -I \"" + folderPath + "\" -I \"" + get_shaderlib_path() + "\"";

        for (auto incl : additionalIncludes)
        {
            includeString += " -I \"" + incl + "\"";
        }

        std::string command = "\"" + get_compiler_path() + "\" \"" + filepath + "\"" + definesString + includeString + " -f 1file -o stdout";

        std::string out, err;

        if (!ShellInvoke(command, out, err))
        {
            m_callback("Shader processor error: " + err, severity::error);
            return "";
        }

        if (!err.empty())
            m_callback("Shader processor warning: " + err, severity::warn);

        out.erase(std::remove(out.begin(), out.end(), '\r'), out.end());

        return out;
    }

    bool ShaderCompiler::process(const fs::view& file, bitfield8 compilerSettings, shader_ilo& ilo, shader_state& state)
    {
        std::vector<std::string> temp;
        return process(file, compilerSettings, ilo, state, temp, temp);
    }

    bool ShaderCompiler::process(const fs::view& file, bitfield8 compilerSettings, shader_ilo& ilo, shader_state& state, const std::vector<std::string>& defines)
    {
        std::vector<std::string> temp;
        return process(file, compilerSettings, ilo, state, defines, temp);
    }

    bool ShaderCompiler::process(const fs::view& file, bitfield8 compilerSettings, shader_ilo& ilo, shader_state& state, const std::vector<std::string>& defines, const std::vector<std::string>& additionalIncludes)
    {
        OPTICK_EVENT();
        using severity = log::severity;

        log::info("Compiling shader: {}", file.get_virtual_path());

        auto result = invoke_compiler(file, compilerSettings, defines, additionalIncludes);

        if (result.empty())
            return false;

        auto start = result.find("=========== BEGIN SHADER CODE ===========\n") + 42;
        start = result.find_first_not_of('\n', start);
        auto end = result.find("============ END SHADER CODE ============");

        auto rest = std::string_view(result.data() + start, end - start);

        while (!rest.empty())
        {
            auto seperator = rest.find_first_not_of('\n');
            if (seperator == std::string::npos)
                break;

            seperator = rest.find_first_of('\n', seperator) + 1;
            if (rest.empty())
                break;
            auto shaderType = std::stoull(std::string(rest.substr(0, seperator)));
            rest = rest.substr(seperator);
            if (rest.empty())
                break;

            seperator = rest.find_first_of('\n') + 1;
            auto sourceLength = std::stoull(std::string(rest.substr(0, seperator)));
            if (sourceLength == 0)
                continue;

            rest = rest.substr(seperator);
            if (rest.empty())
                break;

            seperator = rest.substr(0, sourceLength).find_last_of('\n');
            auto source = rest.substr(0, seperator);
            rest = rest.substr(seperator);

            if (shaderType == 0)
            {
                extract_state(source, state);
            }
            else if(!extract_ilo(source, shaderType, ilo))
            {
                return false;
            }
            if (rest.empty())
                break;

            seperator = rest.find_first_not_of('\n');
            if (seperator == std::string::npos)
                break;
        }

        if (ilo.empty())
        {
            m_callback("Shader processor error: shader was empty", severity::error);
            return false;
        }
        return true;
    }
}
