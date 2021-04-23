#include <core/types/type_util.hpp>
#include <functional>

namespace legion::core
{
    id_type nameHash(const std::string& name)
    {
        OPTICK_EVENT();

#if defined(LEGION_MSVC) || defined(LEGION_CLANG_MSVC)
        static std::hash<std::string> hasher{};
        if (!name.empty() && name[name.size() - 1] == '\0')
            return nameHash(std::string_view(name));

        return hasher(name);
#else
        // std::hash returns a different hash on GCC and Clang on Linux for certain CPU architectures.
        // These certain different hashes are faster to compute but can create issues if they aren't the same.
        return nameHash(std::string_view(name));
#endif
    }

}
