#include <core/serialization/serializationregistry.hpp>
#pragma once

namespace legion::core::serialization
{
    std::map<id_type, std::unique_ptr<serializer_base>> Serialization_Registry::serializers;

    template<typename type>
    inline pointer<serializer<type>> Serialization_Registry::register_serializer()
    {
        Serialization_Registry::serializers.emplace(type_hash<type>().local(), std::make_unique<serializer<type>>());
        return get_serializer<type>();
    }

    template<typename type>
    inline pointer<serializer<type>> Serialization_Registry::get_serializer()
    {
        id_type typeId = type_hash<type>().local();
        if (serializers.count(typeId))
            return { dynamic_cast<serializer<type>*>(serializers.at(typeId).get()) };
        return { nullptr };
    }

    template<typename type>
    inline pointer<serializer<type>> legion::core::serialization::Serialization_Registry::get_serializer(id_type id)
    {
        if (serializers.count(id))
            return { dynamic_cast<serializer<type>*>(serializers.at(id).get()) };
        return { nullptr };
    }
}
