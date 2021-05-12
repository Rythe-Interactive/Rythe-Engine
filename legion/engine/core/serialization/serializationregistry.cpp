#include <core/serialization/serializationregistry.hpp>


namespace legion::core::serialization
{
    std::map<id_type, std::unique_ptr<serializer<void>>> Registry::serializers;

    template<typename type>
    void Registry::register_type()
    {
        Registry::serializers.emplace(type_hash<type>().local(), std::make_unique<serializer<type>>());
    }

    template<typename type>
    std::unique_ptr<serializer<type>> Registry::get_serializer()
    {
        return serializers[type_hash<type>().local()];
    }
}
