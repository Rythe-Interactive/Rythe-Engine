#include <core/serialization/serializer.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename serializable_type>
    inline void serialization::serializer<serializable_type>::serialize(const std::any& serializable, const serializer_view& s_view)
    {
        if (!serializable.has_value())
            return legion_exception_msg("invalid input data: serializable is null");

        if (serializable.type() != typeid(serializable_type))
            return legion_exception_msg("invalid input data: serializable is not of expected type"/*, typeid(serializable_type), typeid(serializable.type()))*/);

        std::string name = typeid(serializable_type).name();
        if (!s_view.serialize<serializable_type>(name, std::any_cast<serializable_type>(serializable)))
        {
            auto reflector = make_reflector(std::any_cast<serializable_type>(serializable));

            for_each(reflector,
                [&s_view](auto& name, auto& value)
                {
                    //if constexpr (!std::is_same<decltype(value), serializable_type>::value)
                    serializer_registry::get_serializer<decltype(value)>().serialize(value, s_view);
                });
        }
    };

    template<typename serializable_type>
    inline prototype_base serialization::serializer<serializable_type>::deserialize(const serializer_view& view)
    {
        return prototype<serializable_type>();
    }
}
