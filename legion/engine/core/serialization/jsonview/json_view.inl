#include <core/serialization/jsonview/json_view.hpp>
#include <core/serialization/serializationregistry.hpp>
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
                using value_type = remove_cvr_t<decltype(value)>;
                if constexpr (std::is_literal_type<value_type>::value/* && !std::is_same<value_type, ecs::entity>::value*/)
                {
                    if constexpr (is_pointer<value_type>::value)
                        j[name] = serialization_util::serialize_property<value_type>(*value);
                    else
                        j[name] = serialization_util::serialize_property<value_type>(value);
                }
                else if constexpr (has_size<value_type>::value)
                {
                    int size = value.size();
                    if constexpr (std::is_same<value_type, std::string>::value)
                        j[name] = value;
                    else
                        j[name] = serialization_util::serialize_container<value_type>(value);
                }
                else
                {
                    using value_type_ptr = remove_cvr_t<decltype(*value)>;
                    if constexpr (is_pointer<value_type>::value)
                        j[name] = json_view<prototype<value_type_ptr>>::serialize(prototype<value_type_ptr>(*value));
                    else
                        j[name] = json_view<prototype<value_type>>::serialize(prototype<value_type>(value));
                }
            });
        return j;
    }

    template<>
    inline json json_view<prototype<ecs::entity>>::serialize(const prototype<ecs::entity> ent_prot)
    {
        json j;
        for_each(ent_prot,
            [&j](auto& name, auto& value)
            {
                j["id"] = value->id;
                j["name"] = value->name;
                j["active"] = (bool)value->active;
                j["alive"] = (bool)value->alive;
            });

        for (int i = 0; i < ent_prot.children.size(); i++)
            j["children"].push_back(json_view<prototype<ecs::entity>>::serialize(ent_prot.children[i]));

        for (auto& [key, value] : ent_prot.composition)
        {
            Serialization_Registry::register_serializer<decltype(value.get())>();
            auto val = *Serialization_Registry::get_serializer(key);
            j["components"].emplace(value.get()->type.global_name().data(), val.serialize(value.get(),JSON));
        }
        return j;
    }

    template<typename prototype_type>
    inline prototype_type json_view<prototype_type>::deserialize(const json j)
    {
        prototype_type prot;
        for_each(prot,
            [&j](auto& name, auto& value)
            {
                using value_type = remove_cvr_t<decltype(value)>;
                if constexpr (std::is_literal_type<value_type>::value)
                    value = serialization_util::deserialize_property<value_type>(j[name]);
                else if constexpr (has_size<value_type>::value)
                    value = serialization_util::deserialize_container<value_type>(j[name]);
                else
                    value = json_view<prototype<value_type>>::deserialize<prototype<value_type>>(j[name]);
            });
        return prot;
    }

    template<>
    inline prototype<ecs::entity> json_view< prototype<ecs::entity>>::deserialize(const json j)
    {
        prototype<ecs::entity> prot;
        //for_each(prot,
        //    [&j](auto& name, auto& value)
        //    {
        //        
        //    });
        return prot;
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

    template<typename property_type>
    inline property_type serialization_util::deserialize_property(const json j)
    {
        return (property_type)j;
    }

    template<typename container_type>
    inline json serialization_util::serialize_container(const container_type prop)
    {
        json j;
        if constexpr (std::is_same<container_type, ecs::entity_data*>::value)
            return j;

        for (int i = 0; i < prop.size(); i++)
            if constexpr (std::is_literal_type<typename container_type::value_type>::value && !std::is_same<typename container_type::value_type, ecs::entity>::value)
                j.push_back(serialize_property<typename container_type::value_type>(prop[i]));
            else
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
