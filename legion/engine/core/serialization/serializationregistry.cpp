#include <core/serialization/serializationregistry.hpp>


namespace legion::core::serialization
{
    std::map<id_type, std::unique_ptr<serializer_base>> SerializationRegistry::serializers;

    std::unique_ptr<component_prototype_base> SerializationRegistry::getPrototype(json j)
    {
        return std::unique_ptr<component_prototype_base>();
    }
}
