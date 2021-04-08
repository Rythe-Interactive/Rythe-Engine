#pragma once
#include <core/serialization/serializationutil.hpp>
#include <core/common/hash.hpp>
#include <map>
#include <any>

namespace legion::core::serialization
{

    class SerializationRegistry
    {
    private:
        static std::map<id_type, serializer_base*> serializers;

    public:
        template<typename type>
        static void register_component()
        {
            SerializationRegistry::serializers.emplace(type_hash<type>().global(), serializer<type>());
        }

        template<typename type>
        static component_prototype<type> getPrototype(type_hash<type>* hash)
        {
            return SerializationRegistry::serializers[hash->global()]->deserialize("{idk}");
        }

        template<typename type>
        static component_prototype<type> getPrototype()
        {
            return SerializationRegistry::serializers[type_hash<type>().global()]->deserialize("{idk}");
        }

    };
}
