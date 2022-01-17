#include <core/serialization/serializationregistry.hpp>

namespace legion::core::serialization
{
    std::unordered_map<id_type, std::unique_ptr<serializer_base>> SerializerRegistry::serializers;
}
