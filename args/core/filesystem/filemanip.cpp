#include <core/filesystem/filemanip.hpp>

namespace args::core::filesystem::literals
{
    byte_vec ARGS_FUNC operator""_readfile(const char* str, std::size_t len)
    {
        return ::args::core::filesystem::read_file(std::string_view(str, len));
    }

    auto ARGS_FUNC operator""_writefile(const char* str, std::size_t len)
    {
        return[path = std::string_view(str, len)](const byte_vec& container)
        {
            ::args::core::filesystem::write_file(path, container);
        };
    }

    bool ARGS_FUNC operator""_exists(const char* str, std::size_t len)
    {
        return ::args::core::filesystem::exists(std::string_view(str, len));
    }
}
