#pragma once
#include <core/serialization/serializationutil.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>
#include <core/common/hash.hpp>
#include <map>
#include <any>

namespace legion::core::serialization
{
    class SerializationRegistry
    {
    public:
        std::map<type_hash_base, serializer_base> serializers;

        template<typename type>
        void register_component()
        {
            auto serializer = json_serializer();
            serializers.emplace(type_hash<type>(),serializer);
        }

        template<typename type>
        type_hash<type> readtypehash(std::string input)
        {
            return type_hash<type>();
        }

        template<typename type>
        prototype<type> deserialize(std::string input)
        {
            type_hash<type> typehash = readtypehash<type>(input);
            return serializers[typehash].deserialize<prototype<type>>(input);
        }
    };
}
