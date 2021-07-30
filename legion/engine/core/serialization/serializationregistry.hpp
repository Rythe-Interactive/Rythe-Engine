#pragma once
#include <map>
#include <any>

#include <core/serialization/serializer.hpp>
#include <core/common/hash.hpp>
#include <core/containers/pointer.hpp>
#include <core/serialization/view_type.hpp>

namespace legion::core::serialization
{
    class Registry
    {
    private:
        static std::map<id_type, std::unique_ptr<serializer_base>> serializers;
    public:
        template<typename type>
        static pointer<serializer<type>> register_serializer();

        template<typename type>
        static pointer<serializer<type>> get_serializer();
    };

}

#include <core/serialization/serializationregistry.inl>


