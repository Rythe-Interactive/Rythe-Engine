#pragma once
#include <map>
#include <any>


#include <core/common/hash.hpp>
#include <core/containers/pointer.hpp>

namespace legion::core::serialization
{

}

#include <core/serialization/serializer.hpp>
namespace legion::core::serialization
{
    template<typename serializablt_type>
    struct serializer;

    class Serialization_Registry
    {
    private:
        static std::map<id_type, std::unique_ptr<serializer_base>> serializers;
    public:
        template<typename type>
        static pointer<serializer<type>> register_serializer();
        template<typename type>
        static pointer<serializer<type>> get_serializer();

        static pointer<serializer_base> get_serializer(id_type id);
    };

}



#include <core/serialization/serializationregistry.inl>


