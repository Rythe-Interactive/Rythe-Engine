#include <core/types/type_util.hpp>
#include <functional>

namespace legion::core
{
    constexpr id_type nameHash(cstring name) noexcept
    {
        id_type hash = 0xcbf29ce484222325;
        constexpr uint64 prime = 0x00000100000001b3;

        for (size_type i = 0; i < common::constexpr_strlen(name); i++)
        {
            hash = hash ^ static_cast<const byte>(name[i]);
            hash *= prime;
        }

        return hash;
    }

    id_type nameHash(const std::string& name)
    {
        OPTICK_EVENT();

#if defined(LEGION_MSVC) || defined(LEGION_CLANG_MSVC)
        static std::hash<std::string> hasher{};
        if (!name.empty() && name[name.size() - 1] == '\0')
            return hasher(name.substr(0, name.size() - 2));

        return hasher(name);
#else
        // std::hash returns a different hash on GCC and Clang on Linux for certain CPU architectures.
        // These certain different hashes are faster to compute but can create issues if they aren't the same.
        return nameHash(std::string_view(name));
#endif
    }

    constexpr id_type nameHash(const std::string_view& name) noexcept
    {
        id_type hash = 0xcbf29ce484222325;
        constexpr uint64 prime = 0x00000100000001b3;

        size_type size = name.size();

        if (name[size - 1] == '\0')
            size--;

        for (size_type i = 0; i < size; i++)
        {
            hash = hash ^ static_cast<const byte>(name[i]);
            hash *= prime;
        }

        return hash;
    }

}
