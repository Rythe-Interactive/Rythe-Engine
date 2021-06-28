#include <core/serialization/serializationregistry.hpp>
#pragma once

namespace legion::core::serialization
{
    std::map<id_type, std::unique_ptr<serializer_base>> Registry::serializers;

    template<typename type>
    inline void Registry::register_type()
    {
        Registry::serializers.emplace(type_hash<type>().local(), std::make_unique<serializer<type>>());
    }

    template<typename type>
    inline pointer<serializer<type>> Registry::get_serializer()
    {
        id_type typeId = type_hash<type>().local();
        if (serializers.count(typeId))
            return { dynamic_cast<serializer<type>*>(serializers.at(typeId).get()) };
        return { nullptr };
    }
}
