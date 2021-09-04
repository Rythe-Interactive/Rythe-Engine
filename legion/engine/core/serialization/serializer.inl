#include <core/serialization/serializer.hpp>
#pragma once

namespace legion::core::serialization
{
    inline void serializer<std::vector<ecs::entity>>::serialize(std::vector<ecs::entity>& serializable, serializer_view& s_view)
    {
        if (!serializable.size() > 0)
        {
            log::error("invalid input data: entity vector is empty");
            return;
        }

        std::string name = typeid(std::vector<ecs::entity>).name();

        for (std::vector<ecs::entity>::const_iterator it = serializable.begin(); it != serializable.end(); ++it)
        {
            auto reflector = make_reflector(*it);

            for_each(reflector,
                [&s_view](auto& name, auto& value)
                {
                    using type = decltype(value);
                    serializer<type> _serializer = *serializer_registry::get_serializer<type>();
                    _serializer.serialize(value, s_view);
                });
        }

    }

    template<typename serializable_type>
    inline void serializer<serializable_type>::serialize(const std::any& serializable, serializer_view& s_view)
    {
        if (!serializable.has_value())
            return legion_exception_msg("invalid input data: serializable is null");

        if (serializable.type() != typeid(serializable_type))
            return legion_exception_msg("invalid input data: serializable is not of expected type");

        std::string name = typeid(serializable_type).name();

        if constexpr (std::is_same_v<remove_cvr_t<serializable_type>, id_type>)
        {
            s_view.serialize<unsigned long long int>(name, std::move(std::any_cast<unsigned long long int>(serializable)));
            return;
        }
        if constexpr (!std::is_constructible_v<std::remove_cv_t<serializable_type>, const remove_cvr_t<serializable_type>&>)
        {
            serializer<serializable_type> _serializer = *serializer_registry::get_serializer<serializable_type>();
            _serializer.serialize(serializable, s_view);
            return;
        }
        else
        {
            serializable_type&& _serializable = std::any_cast<serializable_type>(serializable);
            if (!s_view.serialize<serializable_type>(name, std::move(_serializable)))
            {
                auto reflector = make_reflector<serializable_type>(std::move(_serializable));

                for_each(reflector,
                    [&s_view](auto& name, auto& value)
                    {
                        using type = decltype(value);
                        serializer<type> _serializer = *serializer_registry::get_serializer<type>();
                        _serializer.serialize(value, s_view);
                    });
            }
        }
    }


    template<typename serializable_type>
    inline prototype_base serializer<serializable_type>::deserialize(serializer_view& view)
    {
        return prototype_base();
    }
}
