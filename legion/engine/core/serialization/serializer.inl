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

            name = type_hash<type>().global_name().data();

            s_view.start_container(name);

            for (container_type::const_iterator it = _container->begin(); it != _container->end(); ++it)
            {
                pointer<serializer<value_type>> _serializer = serializer_registry::get_serializer<value_type>();

                _serializer->serialize(&*it, s_view, nameOfType<value_type>());
            }

            s_view.end_container();

            return common::success;
        }

        template<typename type>
        inline common::result<type, fs_error> deserialize_container(serializer_view& s_view)
        {
            using container_type = typename remove_cvr_t<type>;
            using value_type = remove_cvr_t<typename std::iterator_traits<container_type::const_iterator>::value_type>;

            auto container = container_type();

            int size = s_view.start_read_array();
            log::debug("Array Size: "+std::to_string(size));

            if constexpr (has_emplace<container_type,value_type>::value)
            {
                for (int i = 0; i < size; i++)
                {
                    log::debug("Deserializing Container Element");
                    pointer<serializer<value_type>> _serializer = serializer_registry::get_serializer<value_type>();

                    auto result = _serializer->deserialize(s_view, type_hash<value_type>().global_name().data());
                    container.emplace(*static_cast<value_type*>(result.value()));
                }
            }
            else if constexpr (has_push_back<container_type, value_type>::value)
            {
                for (int i = 0; i < size; i++)
                {
                    log::debug("Deserializing Container Element");
                    pointer<serializer<value_type>> _serializer = serializer_registry::get_serializer<value_type>();

                    auto result = _serializer->deserialize(s_view, type_hash<value_type>().global_name().data());
                    container.push_back(*static_cast<value_type*>(result.value()));
                }
            }

            s_view.end_read_array();

            return container;
        }

        inline common::result<void, fs_error> serialize_ent_data(const ecs::entity_data& ent_data, serializer_view& s_view, std::string& name)
        {
            name = type_hash<ecs::entity_data>().global_name().data();

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
                    log::error("Could not find existing serializer for " + compName);
                    continue;
                }
                auto comp = ecs::Registry::getComponent(typeId, ent);
                _serializer->serialize(comp, s_view, std::to_string(typeId));
            }
            s_view.end_container();

            s_view.end_object();

            return common::success;
        }

        inline common::result<ecs::entity, fs_error> deserialize_ent_data(serializer_view& s_view)
        {
            //Entity

            auto ent = ecs::Registry::createEntity();
            log::debug("Begin entity deserialize");
            log::debug("deserialize name");
            ent->name = s_view.deserialize<std::string>("name");
            log::debug("deserialize active");
            ent->active = s_view.deserialize<bool>("active");
            log::debug("deserialize alive");
            ent->alive = s_view.deserialize<bool>("alive");

            //Children
            log::debug("deserialize children");
            auto children = detail::deserialize_container<ecs::entity_set>(s_view);
            for (const auto& e : children.value())
            {
                ent.add_child(e);
            }


            //Components
            log::debug("deserialize components");
            auto components = detail::deserialize_container<std::vector<ecs::component_base>>(s_view);
            for (const auto& c : components.value())
            {
                log::debug("Adding");
                ent.add_component(c);
            }



            return ent;
        }
    }

    inline common::result<void, fs_error>  serializer<ecs::entity_data>::serialize(const void* ent, serializer_view& s_view, std::string name)
    {
        return detail::serialize_ent_data(*static_cast<const ecs::entity_data*>(ent), s_view, name);
    }

    inline common::result<void*, fs_error> serializer<ecs::entity_data>::deserialize(serializer_view& view,std::string name)
    {
        return &detail::deserialize_ent_data(view).value();
    }


    inline common::result<void, fs_error> serializer<ecs::entity>::serialize(const void* ent, serializer_view& s_view, std::string name)
    {
        return detail::serialize_ent_data(
            *static_cast<const ecs::entity_data*>(static_cast<const ecs::entity*>(ent)->data)
            , s_view, name);
    }

    inline common::result<void*, fs_error> serializer<ecs::entity>::deserialize(serializer_view& view,std::string name)
    {
        return &detail::deserialize_ent_data(view).value();
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
        else if constexpr (is_container<serializable_type>::value)
            detail::serialize_container<serializable_type>(serializable, s_view, name);
        else
        {
            auto _serializable = *static_cast<const serializable_type*>(serializable);
            auto reflector = make_reflector(_serializable);

            if (name.size() < 1)
                s_view.start_object();
            else
                s_view.start_object(name);

            for_each(reflector,
                [&s_view](auto& _name, auto& value)
                {
                    using value_type = typename remove_cvr_t<decltype(value)>;
                    if (is_serializable<value_type>::value)
                    {
                        s_view.serialize<value_type>(_name,std::move(value));
                    }
                    else
                    {
                        auto _serializer = serializer_registry::get_serializer<value_type>();
                        _serializer->serialize(&value, s_view, _name);
                    }
                });

            s_view.end_object();
        }
        return common::success;
    }

    template<typename type>
    inline common::result<void*, fs_error> serializer<type>::deserialize(serializer_view& s_view,std::string name)
    {
        using serializable_type = typename remove_cvr_t<type>;

        if constexpr (is_serializable<serializable_type>::value)
        {
            auto val = s_view.deserialize<serializable_type>(name);
            return &val;
        }
        else  if constexpr (is_container<serializable_type>::value)
        {
            auto container = detail::deserialize_container<serializable_type>(s_view).value();
            return &container;
        }
        else
        {
            auto reflector = make_reflector<serializable_type>(serializable_type());
            for_each(reflector,
                [&s_view](auto& name, auto& value)
                {
                    using value_type = typename remove_cvr_t<decltype(value)>;

                    if constexpr (is_serializable<value_type>::value)
                    {
                        value = s_view.deserialize<value_type>(name);
                    }
                    else  if constexpr (is_container<value_type>::value)
                        value = detail::deserialize_container<value_type>(s_view, name).value();
                    else
                    {
                        auto _serializer = serializer_registry::get_serializer<value_type>();
                        if (_serializer)
                            value = *static_cast<value_type>(_serializer->deserialize(s_view,name));
                        else
                            log::error("Serializer can't be created");
                    }
                });
            return &from_reflector(reflector);
        }
    }
}
