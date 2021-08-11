#include <core/serialization/view_type.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename prototype_type>
    inline json json_view<prototype_type>::serialize(const prototype_type prot)
    {
        json j;
        log::debug("Started Serializing prototype");
        for_each(prot,
            [&j](auto& name, auto& value)
            {
                using value_type = remove_cvr_t<decltype(value)>;
                if constexpr (std::is_literal_type<value_type>::value && !is_pointer<value_type>::value && !std::is_same<value_type, ecs::entity>::value)
                {
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
                log::debug("\t Done serializing \n");
            });
        log::debug("Finished Serializing prototype");
        return j;
    }

    template<>
    inline json json_view<prototype<ecs::entity>>::serialize(const prototype<ecs::entity> ent_prot)
    {
        json j;
        log::debug("Started serializing entity prototype");
        for_each(ent_prot,
            [&j](auto& name, auto& value)
            {
                j = json_view<prototype<ecs::entity_data>>::serialize(prototype<ecs::entity_data>(*value));
            });

        for (auto& [id,base] : ent_prot.composition)
        {
            //j["components"].push_back();
        }
        //j["components"] = serialization_util::serialize_container(ent_prot.composition);
        log::debug("Finished serializing entity prototype");
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
                if constexpr (std::is_literal_type<value_type>::value && !is_pointer<value_type>::value && !std::is_same<value_type, ecs::entity>::value)
                    value = serialization_util::deserialize_property<value_type>(j[name]);
                else if constexpr (has_size<value_type>::value)
                    value = serialization_util::deserialize_container<value_type>(j[name]);
                else
                    value = json_view<prototype<value_type>>::deserialize<prototype<value_type>>(j[name]);
            });
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
        if constexpr (!std::is_same<container_type, ecs::entity_data*>::value)
        {
            if constexpr (std::is_same<typename container_type::value_type, ecs::entity>::value || !std::is_literal_type<typename container_type::value_type>::value)
            {
                for (int i = 0; i < prop.size(); i++)
                    j.push_back(json_view<prototype<typename container_type::value_type>>::serialize(prototype<typename container_type::value_type>(prop[i])));
            }
            else if constexpr (std::is_literal_type<typename container_type::value_type>::value)
            {
                for (int i = 0; i < prop.size(); i++)
                    j.push_back(serialize_property<typename container_type::value_type>(prop[i]));
            }
            return j;
        }
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

