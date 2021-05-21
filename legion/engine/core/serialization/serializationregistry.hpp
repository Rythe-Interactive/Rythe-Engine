#pragma once
#include <core/serialization/serializationutil.hpp>
#include <core/common/hash.hpp>
#include <core/containers/pointer.hpp>

#include <map>
#include <any>

namespace legion::core::serialization
{
    class Registry
    {
    private:
        static std::map<id_type, std::unique_ptr<serializer_base>> serializers;
    public:
        template<typename type>
        static void register_type();

        template<typename type>
        static pointer<serializer<type>> get_serializer();
    };

}

#include <core/serialization/serializationregistry.inl>


