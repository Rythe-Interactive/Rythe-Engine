#include <core/serialization/serializationregistry.hpp>


namespace legion::core::serialization
{
    std::map<id_type, std::unique_ptr<serializer_base>> SerializationRegistry::serializers;

    template<typename type>
    void SerializationRegistry::register_component()
    {
        SerializationRegistry::serializers.emplace(type_hash<type>().local(), std::make_unique<serializer<type>>());
    }

    std::unique_ptr<component_prototype_base> SerializationRegistry::getPrototype(json j)
    {
       
        return  serializers[j["Type ID"]]->deserialize(j);
    }
}
