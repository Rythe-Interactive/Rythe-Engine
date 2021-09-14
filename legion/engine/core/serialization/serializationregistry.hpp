#pragma once
#include <core/common/hash.hpp>
#include <core/containers/pointer.hpp>
#include <core/serialization/serializer.hpp>

#include <map>

namespace legion::core::serialization
{
    struct serialized_data
    {
        fs::basic_resource data;
        /*
        * Object:
        * {
        *   Data: value,
        *   Nested_Data:
        [*/
        std::map<std::string,serialized_data> nested_data;
        /*]
        * }
        */
    };

    class serializer_registry
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

#include <core/serialization/serializer.inl>
#include <core/serialization/serializationregistry.inl>


