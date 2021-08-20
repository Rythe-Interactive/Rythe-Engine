#pragma once
#include <core/common/hash.hpp>
#include <core/containers/pointer.hpp>
#include <core/serialization/serializer.hpp>

#include <map>
#include <any>

namespace legion::core::serialization
{
    class Serialization_Registry
    {
    private:
        static std::map<id_type, std::unique_ptr<serializer_base>> serializers;
    public:
        template<typename type>
        static pointer<serializer<type>> register_serializer();
        template<typename type>
        static pointer<serializer<type>> get_serializer();
        template<typename type>
        static pointer<serializer<type>> get_serializer(id_type id);
    };
}

#include <core/serialization/serializer.inl>
#include <core/serialization/serializationregistry.inl>


