#include <core/types/type_util.hpp>
#include <functional>

namespace args::core
{
    id_type ARGS_FUNC nameHash(cstring name)
    {
        id_type hash = 0xcbf29ce484222325;
        uint64 prime = 0x00000100000001b3;
        for (int i = 0; i < std::strlen(name); i++)
        {
            byte value = name[i];
            hash = hash ^ value;
            hash *= prime;
        }

        return hash;
    }

    id_type ARGS_FUNC nameHash(const std::string& name)
    {
        static std::hash<std::string> hasher{};
        return hasher(name);
    }
    id_type ARGS_FUNC nameHash(const std::string_view& name)
    {
        static std::hash<std::string_view> hasher{};
        return hasher(name);
    }
}
