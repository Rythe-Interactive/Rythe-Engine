#include <core/serialization/serializationregistry.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename ObjectType, typename... Args>
    inline pointer<serializer<ObjectType>> serializer_registry::register_serializer(Args&&... args)
    {
        auto [iterator, emplaced] = serializers.try_emplace(make_hash<ObjectType>().id(), std::make_unique<serializer<ObjectType>>(std::forward<Args>(args)...));
        return { dynamic_cast<serializer<ObjectType>*>(iterator->second.get()) };
    }

    template<typename type>
    inline pointer<serializer<type>> serializer_registry::get_serializer()
    {
        static const id_type typeId = make_hash<type>().id();
        if (serializers.count(typeId))
            return { dynamic_cast<serializer<type>*>(serializers.at(typeId).get()) };
        return { nullptr };
    }

    inline pointer<serializer_base> serializer_registry::get_serializer(id_type typeId)
    {
        if (serializers.count(typeId))
            return { serializers.at(typeId).get() };
        return { nullptr };
    }

}


