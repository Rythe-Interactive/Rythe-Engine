#include <core/types/type_util.hpp>
#include <functional>

namespace args::core
{
    id_type ARGS_FUNC nameHash(cstring name)
    {
        const size_type length = std::strlen(name);
        id_type hash = 0xcbf29ce484222325;
        uint64 prime = 0x00000100000001b3;        
        
        for (size_type i = 0; i < length; i++)
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
        if (name[name.size() - 1] == '\0')
        {
            std::string temp = name;
            temp.resize(name.size() - 1);
            return hasher(temp);
        }
        return hasher(name);
    }
    id_type ARGS_FUNC nameHash(const std::string_view& name)
    {
        return nameHash(std::string(name));
    }
}
