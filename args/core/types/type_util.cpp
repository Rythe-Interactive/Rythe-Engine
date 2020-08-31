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

    template<>
    void ARGS_FUNC appendBinaryData(const std::string* value, byte_vec& data)
    {
        std::string copy = *value;
        appendBinaryData(&copy, data);
    }

    template<>
    void ARGS_FUNC appendBinaryData(std::string* value, byte_vec& data)
    {
        appendBinaryData(value->begin(), value->end(), data);
    }

    template<>
    void ARGS_FUNC retrieveBinaryData(std::string& value, byte_vec::const_iterator& start)
    {
        uint64 arrSize = retrieveArraySize<char>(start);
        value.resize(arrSize);

        retrieveBinaryData(value.begin(), value.end(), start);
    }
}
