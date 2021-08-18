#pragma once
#include <map>
#include <any>


#include <core/common/hash.hpp>
#include <core/containers/pointer.hpp>
#include <core/serialization/serializer.hpp>

namespace legion::core::serialization
{
    //struct serializer_base;
    //template<typename serializable_type>
    //struct serializer;

    class Serialization_Registry
    {
    private:
        static std::map<id_type, std::unique_ptr<serializer_base<void*>>> serializers;
    public:
        template<typename type>
        static pointer<serializer<type>> register_serializer();
        template<typename type>
        static pointer<serializer<type>> get_serializer();

        static pointer<serializer_base<void*>> get_serializer(id_type id);
    };
}




#include <core/serialization/serializer.inl>
#include <core/serialization/serializationregistry.inl>


