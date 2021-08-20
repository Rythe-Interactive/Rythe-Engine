#pragma once
#include <core/ecs/handles/entity.hpp>
#include <core/ecs/registry.hpp>

#include <nlohmann/json.hpp>

namespace legion::core::serialization
{
    using json = nlohmann::ordered_json;

    //type should be a prototype
    template<typename prototype_type>
    struct json_view
    {
    public:
        json_view() = default;
        ~json_view() = default;

        //serializes the given prototype into a json string
        static json serialize(const prototype_type prot);

        //deserializes the given json into the approriate prototype
        static prototype_type deserialize(const json j);
    };

    struct serialization_util
    {
        template<typename property_type>
        static json serialize_property(const property_type prop);

        template<typename property_type>
        static property_type deserialize_property(const json j);

        template<typename container_type>
        static json serialize_container(const container_type prop);

        template<typename container_type>
        static container_type deserialize_container(const json j);
    };
}

#include <core/serialization/jsonview/json_view.inl>
