#include <core/serialization/serializer.hpp>
#pragma once

namespace legion::core::serialization
{
    inline common::result<fs::basic_resource, exception> serializer<ecs::entity_data>::serialize_ent_data(const ecs::entity_data& ent_data, serializer_view& s_view)
    {
        if (ent_data.id < 1)
        {
            log::error("invalid input data: entity is null");
            return;
        }

        std::string_view name = typeid(ecs::entity).name();
        s_view.start_object(name);

        s_view.serialize("alive", ent_data.alive);
        s_view.serialize("active", ent_data.active);

        s_view.serialize("id", ent_data.id);
        s_view.serialize("name", ent_data.name);
        if (ent_data.parent)
            s_view.serialize("parent", ent_data.parent->id);

        pointer<serializer<ecs::entity_set>> _serializer = serializer_registry::get_serializer<ecs::entity_set>();
        _serializer->serialize(ent_data.children, s_view);

        return s_view.end_object();
    }
    inline common::result<fs::basic_resource, exception> serializer<ecs::entity_data>::serialize(const std::any& ent, serializer_view& s_view)
    {
        return serialize_ent_data(std::any_cast<ecs::entity_data>(ent), s_view);
    }

    inline common::result<fs::basic_resource, exception> serializer<ecs::entity>::serialize_ent(const ecs::entity& ent, serializer_view& s_view)
    {
        if (ent->id < 1)
        {
            log::error("invalid input data: entity is null");
            return;
        }

        std::string_view name = typeid(ecs::entity).name();
        s_view.start_object(name);
        s_view.serialize("alive", ent->alive);
        s_view.serialize("active", ent->active);

        s_view.serialize("id", ent->id);
        s_view.serialize("name", ent->name);
        if (ent->parent)
            s_view.serialize("parent", ent->parent->id);

        pointer<serializer<ecs::entity_set>> _serializer = serializer_registry::get_serializer<ecs::entity_set>();
        _serializer->serialize(ent->children, s_view);

        return s_view.end_object();

    }
    inline common::result<fs::basic_resource, exception> serializer<ecs::entity>::serialize(const std::any& ent, serializer_view& s_view)
    {
        return serialize_ent(std::any_cast<ecs::entity>(ent), s_view);
    }

    template<typename type>
    inline common::result<fs::basic_resource, exception> serializer<type>::serialize_container(const std::any& container, serializer_view& s_view)
    {
        using container_type = typename remove_cvr_t<type>;

        std::string_view strv("Collection");
        s_view.start_object(strv);
        auto _container = std::any_cast<container_type>(container);

        for (container_type::const_iterator it = _container.begin(); it != _container.end(); ++it)
        {
            using value_type = remove_cvr_t<decltype(*it._Ptr)>;
            pointer<serializer<value_type>> _serializer = serializer_registry::get_serializer<value_type>();
            _serializer->serialize(*it._Ptr, s_view);
        }
        return s_view.end_object();
    }

    template<typename type>
    inline common::result<fs::basic_resource, exception> serializer<type>::serialize(const std::any& serializable, serializer_view& s_view)
    {
        using serializable_type = typename remove_cvr_t<type>;


        if (!serializable.has_value())
            return legion_exception_msg("invalid input data: serializable is null");

        //if (serializable.type() != typeid(serializable_type))
        //    return legion_exception_msg("invalid input data: serializable is not of expected type");

        std::string_view name = typeid(serializable_type).name();
        s_view.start_object(name);

        if constexpr (std::is_same<serializable_type, id_type>::value)
            s_view.serialize<unsigned long long int>(name, std::any_cast<unsigned long long int>(serializable));
        else if constexpr (is_container<type>::value)
            return serialize_container(serializable, s_view);
        else if constexpr (!std::is_constructible<serializable_type, const serializable_type&>::value)
        {
            pointer<serializer<serializable_type>> _serializer = serializer_registry::get_serializer<serializable_type>();
            _serializer->serialize(serializable, s_view);
        }
        else
        {
            auto _serializable = std::any_cast<serializable_type>(serializable);
            if (!s_view.serialize<serializable_type>(name, std::move(_serializable)))
            {
                auto reflector = make_reflector(_serializable);

                for_each(reflector,
                    [&s_view](auto& name, auto& value)
                    {
                        using value_type = typename remove_cvr_t<decltype(value)>;
                        auto _serializer = serializer_registry::get_serializer<value_type>();
                        _serializer->serialize(value, s_view);
                    });
            }
        }
        serialized_data sd{ s_view.end_object() };
        return sd.data;
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
