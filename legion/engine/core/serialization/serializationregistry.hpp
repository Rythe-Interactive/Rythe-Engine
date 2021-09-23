#pragma once
#include <core/common/hash.hpp>
#include <core/containers/pointer.hpp>
#include <core/serialization/serializer.hpp>

#include <map>

namespace legion::core::serialization
{
    class serializer_registry
    {
    private:
        static std::unordered_map<id_type, std::unique_ptr<serializer_base>> serializers;
    public:
        template<typename type>
        static pointer<serializer<type>> register_serializer();

        template<typename type>
        static pointer<serializer<type>> get_serializer();
        static pointer<serializer_base> get_serializer(id_type typeId);
    };
}

#include <core/serialization/serializer.inl>
#include <core/serialization/serializationregistry.inl>

