#include <core/serialization/serializer.hpp>
#pragma once

namespace legion::core::serialization
{
    namespace detail
    {
        template<typename type>
        inline common::result<void, fs_error> serialize_container(const void* container, serializer_view& s_view, std::string& name)
        {
            using container_type = typename remove_cvr_t<type>;
            using value_type = remove_cvr_t<typename std::iterator_traits<container_type::const_iterator>::value_type>;

            auto _container = static_cast<container_type*>(const_cast<void*>(container));

            s_view.start_container(name);

            for (container_type::const_iterator it = _container->begin(); it != _container->end(); ++it)
            {
                pointer<serializer<value_type>> _serializer = serializer_registry::get_serializer<value_type>();

                _serializer->serialize(&*it, s_view, nameOfType<value_type>());
            }

            s_view.end_container();
        }

        inline common::result<void, fs_error> serialize_ent_data(const ecs::entity_data& ent_data, serializer_view& s_view, std::string& name)
        {
            s_view.start_object(name);

            s_view.serialize("name", ent_data.name);

            s_view.serialize("alive", ent_data.alive);
            s_view.serialize("active", ent_data.active);

            pointer<serializer<ecs::entity_set>> _serializer = serializer_registry::get_serializer<ecs::entity_set>();
            std::string container_name = "children";
            detail::serialize_container<ecs::entity_set>(&ent_data.children, s_view, container_name);

            s_view.start_container("components");
            auto ent_composition = ecs::Registry::entityComposition(ent_data.id);

            for (id_type typeId : ent_composition)
            {
                auto ent = ecs::Registry::getEntity(ent_data.id);;
                auto _serializer = serializer_registry::get_serializer(typeId);
                std::string compName = ecs::Registry::getFamilyName(typeId);
                if (!_serializer)
                {
                    log::error("Could not find existing serializer for "+compName);
                    continue;
                }
                auto comp = ecs::Registry::getComponent(typeId, ent);
                _serializer->serialize(comp, s_view, compName);
            }
            s_view.end_container();

            s_view.end_object();
        }

    }

    inline common::result<void, fs_error>  serializer<ecs::entity_data>::serialize(const void* ent, serializer_view& s_view, std::string name)
    {
        return detail::serialize_ent_data(*static_cast<const ecs::entity_data*>(ent), s_view, name);
    }

    inline common::result<void, fs_error> serializer<ecs::entity>::serialize(const void* ent, serializer_view& s_view, std::string name)
    {
        return detail::serialize_ent_data(
            *static_cast<const ecs::entity_data*>(static_cast<const ecs::entity*>(ent)->data)
            , s_view, name);
    }

    template<typename type>
    inline common::result<void, fs_error> serializer<type>::serialize(const void* serializable, serializer_view& s_view, std::string name)
    {
        using serializable_type = typename remove_cvr_t<type>;

        if constexpr (is_serializable<serializable_type>::value)
        {
            auto _serializable = *static_cast<const serializable_type*>(serializable);
            s_view.serialize<serializable_type>(name, std::move(_serializable));
        }
        else if constexpr (is_container<type>::value)
            detail::serialize_container<type>(serializable, s_view, name);
        else
        {
            auto _serializable = *static_cast<const type*>(serializable);
            auto reflector = make_reflector(_serializable);

            if (name.size() < 1)
                s_view.start_object();
            else
                s_view.start_object(name);

            for_each(reflector,
                [&s_view](auto& _name, auto& value)
                {
                    using value_type = typename remove_cvr_t<decltype(value)>;

                    auto _serializer = serializer_registry::get_serializer<value_type>();
                    _serializer->serialize(&value, s_view, _name);
                });

            s_view.end_object();
        }
    }


    inline prototype_base serializer<ecs::entity_data>::deserialize(serializer_view& view)
    {
        return prototype_base();
    }

    inline prototype_base serializer<ecs::entity>::deserialize(serializer_view& view)
    {
        return prototype_base();
    }

    template<typename serializable_type>
    inline prototype_base serializer<serializable_type>::deserialize(serializer_view& view)
    {
        return prototype_base();
    }
}
