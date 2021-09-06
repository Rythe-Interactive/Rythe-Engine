#include <core/serialization/serializer.hpp>
#pragma once

namespace legion::core::serialization
{
    inline void serializer<std::vector<ecs::entity>>::serialize(const std::any& serializable, serializer_view& s_view)
    {
        serialize_vec(std::any_cast<std::vector<ecs::entity>>(serializable),s_view);
    }
    inline void serializer<std::vector<ecs::entity>>::serialize_vec(const std::vector<ecs::entity>& serializable, serializer_view& s_view)
    {
        if (!serializable.size() > 0)
        {
            log::error("invalid input data: entity vector is empty");
            return;
        }

        std::string name = typeid(std::vector<ecs::entity>).name();

        for (std::vector<ecs::entity>::const_iterator it = serializable.begin(); it != serializable.end(); ++it)
        {
            auto reflector = make_reflector(*it._Ptr);

            for_each(reflector,
                [&s_view](auto& name, auto& value)
                {
                    using type = remove_cvr_t<decltype(value)>;
                    pointer<serializer<type>> _serializer = serializer_registry::get_serializer<type>();
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

        if (serializable.type() != typeid(type))
            return legion_exception_msg("invalid input data: serializable is not of expected type");

        std::string name = typeid(type).name();

        if constexpr (std::is_same<serializable_type, id_type>::value)
        {
            s_view.serialize<unsigned long long int>(name, std::any_cast<unsigned long long int>(serializable));
            return;
        }
        else if constexpr (std::is_constructible<serializable_type, const serializable_type&>::value)
        {
            serializable_type _serializable = std::any_cast<serializable_type>(serializable);
            if (!s_view.serialize<serializable_type>(name, std::move(_serializable))) 
            {
                auto reflector = make_reflector(_serializable);

                for_each(reflector,
                    [&s_view](auto& name, auto& value)
                    {
                        using type = remove_cvr_t<decltype(value)>;
                        auto _serializer = serializer_registry::get_serializer<type>();
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
