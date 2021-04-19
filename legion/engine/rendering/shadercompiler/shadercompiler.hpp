#pragma once
#include <rendering/data/shader.hpp>
#include <core/core.hpp>

namespace legion::rendering
{
    class ShaderCompiler
    {
    private:
        static delegate<void(const std::string&, log::severity)> m_callback;

        static std::string get_view_path(const fs::view& view, bool mustBeFile = false);
        static const std::string& get_shaderlib_path();
        static const std::string& get_compiler_path();
        static const std::string& get_cachecleaner_path();

        static void extract_state(std::string_view source, shader_state& state);
        static bool extract_ilo(const std::string& variant, std::string_view source, uint64 shaderType, shader_ilo& ilo);
        static std::string invoke_compiler(const fs::view& file, bitfield8 compilerSettings, const std::vector<std::string>& defines, const std::vector<std::string>& additionalIncludes);

    public:
        template<class owner_type, void(owner_type::* func_type)(const std::string&, log::severity)>
        static void setErrorCallback(owner_type* instance = nullptr)
        {
            m_callback = delegate<void(const std::string&, log::severity)>::template from<owner_type, func_type>(instance);
        }

        template<class owner_type, void(owner_type::* func_type)(const std::string&, log::severity) const>
        static void setErrorCallback(const owner_type* instance)
        {
            m_callback = delegate<void(const std::string&, log::severity)>::template from<owner_type, func_type>(instance);
        }

        static void setErrorCallback(const delegate<void(const std::string&, log::severity)>& func)
        {
            m_callback = func;
        }

        template<typename lambda_type CNDOXY(std::enable_if_t<!std::is_same_v<std::remove_reference_t<lambda_type>, delegate<void(const std::string&, log::severity)>>, int> = 0)>
        static void setErrorCallback(const lambda_type& lambda)
        {
            m_callback = lambda;
        }

        template <void(*func_type)(const std::string&, log::severity)>
        static void setErrorCallback()
        {
            m_callback = delegate<void(const std::string&, log::severity)>::template from<func_type>();
        }

        static void cleanCache();

        static bool process(const fs::view& file, bitfield8 compilerSettings, shader_ilo& ilo, std::unordered_map<std::string, shader_state>& state);
        static bool process(const fs::view& file, bitfield8 compilerSettings, shader_ilo& ilo, std::unordered_map<std::string, shader_state>& state, const std::vector<std::string>& defines);
        static bool process(const fs::view& file, bitfield8 compilerSettings, shader_ilo& ilo, std::unordered_map<std::string, shader_state>& state, const std::vector<std::string>& defines, const std::vector<std::string>& additionalIncludes);
    };
}
