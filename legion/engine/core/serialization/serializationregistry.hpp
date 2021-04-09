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
        static std::map<id_type, std::unique_ptr<serializer_base>> serializers;

    public:
        template<typename type>
        static void register_component()
        {
            SerializationRegistry::serializers.emplace(type_hash<type>().global(), serializer<type>());
        }

        //static std::un getPrototype(type_hash<type>* hash)
        //{
        //    return SerializationRegistry::serializers[hash->global()]->deserialize("{idk}").get();
        //}

        static std::unique_ptr<component_prototype_base> getPrototype(std::string json)
        {
            return SerializationRegistry::serializers[type_hash<type>().global()]->deserialize("{idk}");
        }

    };
}
