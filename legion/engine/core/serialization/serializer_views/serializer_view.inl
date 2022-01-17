#include <core/serialization/serializer_views/serializer_view.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename Type>
    inline bool serializer_view::serialize(const std::string& name, Type&& value)
    {
        using raw_type = std::decay_t<Type>;

        if constexpr (std::is_same_v<raw_type, int>)
        {
            serialize_int(name, value);
            return true;
        }
        else if constexpr (std::is_same_v<raw_type, float>)
        {
            serialize_float(name, value);
            return true;
        }
        else if constexpr (std::is_same_v<raw_type, double>)
        {
            serialize_double(name, value);
            return true;
        }
        else if constexpr (std::is_same_v<raw_type, bool>)
        {
            serialize_bool(name, value);
            return true;
        }
        else if constexpr (std::is_same_v<raw_type, std::string>)
        {
            serialize_string(name, value);
            return true;
        }
        else if constexpr (std::is_same_v<raw_type, id_type>)
        {
            serialize_id_type(name, value);
            return true;
        }
        return false;
    }

    template<typename Type>
    inline common::result<Type, fs_error> serializer_view::deserialize(const std::string& name)
    {
        using raw_type = std::decay_t<Type>;

        if constexpr (std::is_same_v<raw_type, int>)
        {
            return deserialize_int(name);
        }
        else if constexpr (std::is_same_v<raw_type, float>)
        {
            return deserialize_float(name);
        }
        else if constexpr (std::is_same_v<raw_type, double>)
        {
            return deserialize_double(name);
        }
        else if constexpr (std::is_same_v<raw_type, bool>)
        {
            return deserialize_bool(name);
        }
        else if constexpr (std::is_same_v<raw_type, std::string>)
        {
            return deserialize_string(name);
        }
        else if constexpr (std::is_same_v<raw_type, id_type>)
        {
            return deserialize_id_type(name);
        }
        return legion_fs_error("Type was not a primitive serializable type.");
    }
}
