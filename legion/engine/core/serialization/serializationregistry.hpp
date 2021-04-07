#pragma once
#include <core/serialization/serializationutil.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>
#include <core/common/hash.hpp>
#include <string>
#include <map>
#include <any>

namespace legion::core::serialization
{
    class SerializationRegistry
    {
    public:
        std::map<type_hash<MyRecord>, serializer_base> serializers;

        template<typename type>
        void register_component()
        {
            //static_assert(std::is_base_of<ecs::component<MyRecord>, type>::value, "type must derive from ecs::component_base");
            auto serializer = json_serializer();
            serializers.emplace(type_hash<type>(),serializer);
        }

        template<typename type>
        type_hash<type> readtypehash(std::string input)
        {
            return type_hash<type>();
        }

        template<typename type>
        prototype<type> deserialize(std::string input)
        {
            type_hash<type> typehash = readtypehash<type>(input);
            log::debug("heyo");
            return serializers[typehash].deserialize<prototype<type>>(input);
        }
    };
}
