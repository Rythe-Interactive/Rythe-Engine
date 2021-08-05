#include <core/serialization/view_type.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename prototype_type>
    inline json json_view<prototype_type>::serialize(const prototype_type prot)
    {
        json j;
        for_each(prot,
            [&j](auto& name, auto& value)
            {
                if constexpr (std::is_literal_type<remove_cvr_t<decltype(value)>>::value && !std::is_same<decltype(value),ecs::entity>::value)
                {
                    log::debug(std::string("Name:" + name));
                    j[name] = serialization_util::serialize_property(value);
                }
                else if constexpr (has_size<remove_cvr_t<decltype(value)>>::value)
                {
                    log::debug(std::string("Name:" + name));
                    int size = value.size();
                    log::debug(std::string("Size:" + std::to_string(size)));
                    j[name] = serialization_util::serialize_container(value);
                }
                else
                {
                    log::debug(std::string("Name:" + name));
                    log::debug(value->id);
                    j[name] = json_view<prototype<remove_cvr_t<decltype(value)>>>::serialize(prototype<remove_cvr_t<decltype(value)>>(value));
                }
            });
        return j;
    }

    template<typename prototype_type>
    inline prototype_type json_view<prototype_type>::deserialize(const json j)
    {
        prototype_type prot;
        for_each(prot,
            [&j](auto& name, auto& value)
            {

                if constexpr (std::is_literal_type<decltype(value)>::value)
                    value = serialization_util::deserialize_property<remove_cvr_t<decltype(value)>>(j[name]);
                else if constexpr (has_size<decltype(value)>::value)
                    value = serialization_util::deserialize_container<remove_cvr_t<decltype(value)>>(j[name]);
                else
                    value = json_view<prototype<decltype(value)>>::deserialize(j[name]);
            });
        return prot;
    }


    //template<>
    //inline json serialization_util::serialize_property(const ecs::entity prop)
    //{
    //    entity_prototype ent_prot(prop);
    //    return json_view<entity_prototype>::serialize(ent_prot);
    //    //json j;
    //    //j["id"] = prop->id;
    //    //j["name"] = ent_prot.name;
    //    //j["alive"] = prop->alive;
    //    //j["active"] = ent_prot.active;
    //    //if (prop->parent)
    //    //    j["parent"] = prop->parent->id;
    //    //auto children = prop->children;
    //    //for (int i = 0; i < children.size(); i++)
    //    //{
    //    //    auto child_j = json_view<entity_prototype>
    //    //    //auto child_j = serialize_property(children.at(i));
    //    //    j["children"].push_back(child_j);
    //    //}
    //    //auto& components = ent_prot.composition;
    //    //for (auto&[id,comp_prot] : components)
    //    //{
    //    //    j["components"] = json_view<decltype(*comp_prot)>::serialize(*comp_prot);
    //    //}
    //    //return j;
    //}

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
        //if (ecs::Registry::checkEntity(deserialize_property<id_type>(j["id"])))
        //    ent = ecs::Registry::getEntity(deserialize_property<id_type>(j["id"]));
        //else
        //{
        //    ent = ecs::Registry::createEntity();
        //    ent->id = j["id"];
        //}
        //ent->name = j["name"];
        //ent->alive = j["alive"];
        //ent->active = j["active"];
        //if (ecs::Registry::checkEntity(deserialize_property<id_type>(j["parent"])))
        //    ent.set_parent(ecs::Registry::getEntity(deserialize_property<id_type>(j["parent"])));
        //else
        //{
        //    ent.set_parent(ecs::Registry::createEntity());
        //    ent->parent->id = deserialize_property<id_type>(j["parent"]);
        //}
        //ent->children = deserialize_container<remove_cvr_t<ecs::entity_set>>(j["children"]);
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
        if constexpr (std::is_literal_type<typename container_type::value_type>::value && !std::is_same<typename container_type::value_type, ecs::entity>::value)
            for (int i = 0; i < prop.size(); i++)
                j.push_back(serialize_property<typename container_type::value_type>(prop[i]));
        else
            for (int i = 0; i < prop.size(); i++)
                j.push_back(json_view<prototype<typename container_type::value_type>>::serialize(prototype<typename container_type::value_type>(prop[i])));
        return j;
    }

    template<typename container_type>
    inline container_type serialization_util::deserialize_container(json j)
    {
        container_type c_type{};
        for (json::iterator it = j.begin(); it != j.end(); ++it)
            c_type.push_back(deserialize_property<typename container_type::value_type>(it.value()));
        return c_type;
    }

}

