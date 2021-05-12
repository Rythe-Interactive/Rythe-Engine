#pragma once
#include <core/serialization/serializationutil.hpp>
#include <core/common/hash.hpp>
#include <map>
#include <any>

namespace legion::core::serialization
{

    class Registry
    {
    private:
        static std::map<id_type, std::unique_ptr<serializer<void>>> serializers;

    public:
        template<typename type>
        static void register_type();

        template<typename type>
        static std::unique_ptr<serializer<type>> get_serializer();
    };
}
