#pragma once
#include <core/serialization/serializationutil.hpp>
#include <core/common/hash.hpp>
#include <map>
#include <any>

namespace legion::core::serialization
{

    class SerializationRegistry
    {
    private:
        static std::map<id_type, std::unique_ptr<serializer_base>> serializers;

    public:
        template<typename type>
        static void register_component()
        {
            SerializationRegistry::serializers.emplace(0, std::make_unique<serializer<type>>());
        }

        static std::unique_ptr<component_prototype_base> getPrototype(json j);


    };
}
