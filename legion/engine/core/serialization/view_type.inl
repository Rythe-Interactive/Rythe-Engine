#include <core/serialization/view_type.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename type>
    inline json json_view<type>::serialize(const type prot)
    {
        json j;
        for_each(prot,
            [&j](auto& name, auto& value)
            {
                if constexpr (!has_size<decltype(value)>::value)
                    j[name] = serialization_util::serialize_property(value);
                else
                    j[name] = serialization_util::serialize_container(value);
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
                if constexpr (!has_size<decltype(value)>::value)
                    value = serialization_util::deserialize_property<remove_cvr_t<decltype(value)>>(j[name]);
                else
                    value = serialization_util::deserialize_container<remove_cvr_t<decltype(value)>>(j[name]);
            });
        return prot;
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
        //auto components = prop.component_composition();
        //for (id_type comp_id : components)
        //{
        //    auto type_ref = type_ref_cast(comp_id);
        //    auto component = ecs::Registry::getComponent(comp_id,prop);
        //    j["components"].push_back(serialize_property<decltype(component)>(component));
        //}
        //json j = json_view<prototype<ecs::entity>>::serialize(prototype<ecs::entity>(prop));
        return j;
    }

    template<typename property_type>
    inline json serialization_util::serialize_property(const property_type prop)
    {
        return prop;
    }

    template<>
    inline id_type serialization_util::deserialize_property(const json j)
    {
        int int_id = j;
        id_type id = (id_type)int_id;
        return id;
    }

    template<>
    inline ecs::entity serialization_util::deserialize_property(const json j)
    {
        ecs::entity ent;
        if (ecs::Registry::checkEntity(deserialize_property<id_type>(j["id"])))
            ent = ecs::Registry::getEntity(deserialize_property<id_type>(j["id"]));
        else
        {
            ent = ecs::Registry::createEntity();
            ent->id = j["id"];
        }
        ent->name = j["name"];
        ent->alive = j["alive"];
        ent->active = j["active"];
        if (ecs::Registry::checkEntity(deserialize_property<id_type>(j["parent"])))
            ent.set_parent(ecs::Registry::getEntity(deserialize_property<id_type>(j["parent"])));
        else
        {
            ent.set_parent(ecs::Registry::createEntity());
            ent->parent->id = deserialize_property<id_type>(j["parent"]);
        }
        ent->children = deserialize_container<remove_cvr_t<ecs::entity_set>>(j["children"]);
        return ent;
    }

    template<typename property_type>
    inline property_type serialization_util::deserialize_property(const json j)
    {
        return (property_type)j;
    }


    template<typename container_type>
    inline json serialization_util::serialize_container(const container_type prop)
    {
        json j;
        for (int i = 0; i < prop.size(); i++)
        {
            j.push_back(serialize_property<typename container_type::value_type>(prop[i]));
        }
        return j;
    }

    template<typename container_type>
    inline container_type serialization_util::deserialize_container(json j)
    {
        container_type c_type {};
        for (json::iterator it = j.begin(); it != j.end(); ++it)
        {
            c_type.push_back(deserialize_property<typename container_type::value_type>(it.value()));
        }
        return c_type;
    }

}

