#include <core/serialization/serializer.hpp>
#pragma once

namespace legion::core::serialization
{
    inline void serializer<ecs::entity>::serialize_ent(const ecs::entity& serializable, serializer_view& s_view)
    {
        if (serializable->id < 1)
        {
            log::error("invalid input data: entity is null");
            return;
        }

        std::string name = typeid(ecs::entity).name();

        auto reflector = make_reflector(serializable);//i should move this to a member variable that I can just reuse

        for_each(reflector,
            [&s_view](auto& name, auto& value)
            {
                using value_type = typename remove_cvr_t<decltype(*value)>;
                pointer<serializer<value_type>> _serializer = serializer_registry::get_serializer<value_type>();
                _serializer->serialize(*value, s_view);
            });
    }

    inline void serializer<ecs::entity>::serialize(const std::any& serializable, serializer_view& s_view)
    {
        serialize_ent(std::any_cast<ecs::entity>(serializable), s_view);
    }

    template<typename type>
    inline void serializer<type>::serialize_container(const std::any& container, serializer_view& s_view)
    {
        using container_type = typename remove_cvr_t<type>;

        auto& _container = std::any_cast<container_type>(container);

        if (_container.size() < 1)
        {
            log::error("invalid input data: container is empty");
            return;
        }

        std::string name = typeid(type).name();

        for (container_type::const_iterator it = _container.begin(); it != _container.end(); ++it)
        {
            auto reflector = make_reflector(*it._Ptr);

            for_each(reflector,
                [&s_view](auto& name, auto& value)
                {
                    using value_type = remove_cvr_t<decltype(value)>;
                    pointer<serializer<value_type>> _serializer = serializer_registry::get_serializer<value_type>();
                    _serializer->serialize(value, s_view);
                });
        }
    }

    template<typename type>
    inline void serializer<type>::serialize(const std::any& serializable, serializer_view& s_view)
    {
        using serializable_type = remove_cvr_t<type>;

        if (!serializable.has_value())
            return legion_exception_msg("invalid input data: serializable is null");

        if (serializable.type() != typeid(serializable_type))
            return legion_exception_msg("invalid input data: serializable is not of expected type");

        std::string name = typeid(serializable_type).name();

        if constexpr (std::is_same<serializable_type, id_type>::value)
            s_view.serialize<unsigned long long int>(name, std::any_cast<unsigned long long int>(serializable));
        else if constexpr (is_container<remove_cvr_t<type>>::value)
            serialize_container(serializable, s_view);
        else if constexpr (std::is_constructible<serializable_type, const serializable_type&>::value)
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
        else
        {
            pointer<serializer<serializable_type>> _serializer = serializer_registry::get_serializer<serializable_type>();
            _serializer->serialize(serializable, s_view);
            return;
        }
    }


    template<typename serializable_type>
    inline prototype_base serializer<serializable_type>::deserialize(serializer_view& view)
    {
        return prototype_base();
    }
}
