#include <core/filesystem/filemanip.hpp>

namespace legion::core::filesystem::literals
{
    byte_vec LEGION_FUNC operator""_readfile(const char* str, std::size_t len)
    {
        return ::legion::core::filesystem::read_file(std::string_view(str, len));
    }

    auto LEGION_FUNC operator""_writefile(const char* str, std::size_t len)
    {
        return[path = std::string_view(str, len)](const byte_vec& container)
        {
            ::legion::core::filesystem::write_file(path, container);
        };
    }

    bool LEGION_FUNC operator""_exists(const char* str, std::size_t len)
    {
        return ::legion::core::filesystem::exists(std::string_view(str, len));
    }
}
