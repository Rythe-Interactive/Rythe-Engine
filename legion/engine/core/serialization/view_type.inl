#include <core/serialization/view_type.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename prototype>
    inline json json_view<prototype>::serialize(const prototype object)
    {
        json j;
        for_each(object,
            [&j](auto& name, auto& value)
            {
                j[name] = serialization_util::serialize_property(value);
            });
        return j;
    }

    template<typename prototype>
    inline prototype json_view<prototype>::deserialize(const json j)
    {
        prototype prot;
        for_each(prot,
            [&j](auto& name, auto& value)
            {
                value = serialization_util::deserialize_property<decltype(value)>(j[name]);
            });
        return prot;
    }

    template<typename property_type>
    inline json serialization_util::serialize_property(const property_type prop)
    {
        return prop;
    }

    template<>
    inline json serialization_util::serialize_property(const ecs::entity prop)
    {
        json j;
        j["id"] = prop->id;
        j["name"] = prop->name;
        j["alive"] = prop->alive;
        j["active"] = prop->active;
        if (prop->parent)
            j["parent"] = prop->parent->id;
        auto children = prop->children;
        for (int i = 0; i < children.size(); i++)
        {
            j["children"].push_back(serialize_property<ecs::entity>(children.at(i)));
        }
        return j;
    }

    template<>
    inline json serialization_util::serialize_property<std::vector<ecs::entity>>(const std::vector<ecs::entity> prop)
    {
        json j;
        for (int i = 0; i < prop.size(); i++)
        {
            j.push_back(serialize_property<ecs::entity>(prop[i]));
        }
        return j;
    }

    template<typename property_type>
    inline property_type serialization_util::deserialize_property(const json j)
    {
        return (property_type)j;
    }

    template<>
    inline id_type serialization_util::deserialize_property(const json j)
    {
        int int_id = j;
        id_type id = (id_type)int_id;
        return id;
    }
}

