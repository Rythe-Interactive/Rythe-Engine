#include <core/serialization/serializer_view.hpp>
#pragma once

namespace legion::core::serialization
{
#pragma region serializer_view
    template<typename Type>
    inline bool serializer_view::serialize(std::string name, Type&& value)
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
#pragma endregion

#pragma region json_view
    inline void json_view::start_object(std::string name)
    {
        current_writing.emplace("\"name\":{}"_json);
    }

    inline void json_view::end_object()
    {
        if (current_writing.empty())
            return;

        auto cur = current_writing.top();

        if (!cur.is_object())
            return;

        current_writing.pop();
        if (current_writing.empty())
        {
            root.emplace(cur);
        }
        else
        {
            auto& next = current_writing.top();

            if (next.is_array())
                next.emplace_back(cur);
            else if (next.is_object())
                next.emplace(cur);
        }
    }

    void json_view::serialize_int(std::string& name, int serializable)
    {
        json j;
        j[name] = serializable;
        current_writing.emplace(j);
    }

    void json_view::serialize_float(std::string& name, float serializable)
    {
        json j;
        j[name] = serializable;
        current_writing.emplace(j);
    }

    void json_view::serialize_double(std::string& name, double serializable)
    {
        json j;
        j[name] = serializable;
        current_writing.emplace(j);
    }

    void json_view::serialize_bool(std::string& name, bool serializable)
    {
        json j;
        j[name] = serializable;
        current_writing.emplace(j);
    }

    void json_view::serialize_string(std::string& name, const std::string_view& serializable)
    {
        json j;
        j[name] = serializable;
        current_writing.emplace(j);
    }

    void json_view::serialize_id_type(std::string& name, id_type serializable)
    {
        json j;
        j[name] = (int)serializable;
        current_writing.emplace(j);
    }

    inline common::result<void, fs_error> json_view::write(const void* serializable,std::string name,fs::view& file)
    {
        auto serializer = serializer_registry::get_serializer<void*>();
        auto result = serializer->serialize(serializable, this, name);

        if (!result)
            return legion_fs_error("Serialization failed");

        return file.set(fs::basic_resource(root.dump()));
    }

    common::result<int, exception> json_view::deserialize_int(std::string_view& name)
    {
        return legion_exception_msg("not implemented");
    }

    common::result<float, exception> json_view::deserialize_float(std::string_view& name)
    {
        return legion_exception_msg("not implemented");
    }

    common::result<double, exception> json_view::deserialize_double(std::string_view& name)
    {
        return legion_exception_msg("not implemented");
    }

    bool json_view::deserialize_bool(std::string_view& name)
    {
        return false;
    }

    common::result<std::string, exception> json_view::deserialize_string(std::string_view& name)
    {
        return legion_exception_msg("not implemented");
    }

    common::result<id_type, exception> json_view::deserialize_id_type(std::string_view& name)
    {
        return legion_exception_msg("not implemented");
    }
#pragma endregion

#pragma region bson_view
    void bson_view::serialize_int(std::string& name, int serializable)
    {

    }
    void bson_view::serialize_float(std::string& name, float serializable)
    {

    }
    void bson_view::serialize_double(std::string& name, double serializable)
    {

    }
    void bson_view::serialize_bool(std::string& name, bool serializable)
    {

    }
    void bson_view::serialize_string(std::string& name, const std::string_view& serializable)
    {

    }
    void bson_view::serialize_id_type(std::string& name, id_type serializable)
    {

    }

    common::result<int, exception> bson_view::deserialize_int(std::string_view& name)
    {
        return;
    }
    common::result<float, exception> bson_view::deserialize_float(std::string_view& name)
    {
        return;
    }
    common::result<double, exception> bson_view::deserialize_double(std::string_view& name)
    {
        return;
    }
    bool bson_view::deserialize_bool(std::string_view& name)
    {
        return;
    }
    common::result<std::string, exception> bson_view::deserialize_string(std::string_view& name)
    {
        return;
    }
    common::result<id_type, exception> bson_view::deserialize_id_type(std::string_view& name)
    {
        return;
    }
#pragma endregion

#pragma region yaml_view
    void yaml_view::serialize_int(std::string& name, int serializable)
    {

    }
    void yaml_view::serialize_float(std::string& name, float serializable)
    {

    }
    void yaml_view::serialize_double(std::string& name, double serializable)
    {

    }
    void yaml_view::serialize_bool(std::string& name, bool serializable)
    {

    }
    void yaml_view::serialize_string(std::string& name, const std::string_view& serializable)
    {

    }
    void yaml_view::serialize_id_type(std::string& name, id_type serializable)
    {

    }

    common::result<int, exception> yaml_view::deserialize_int(std::string_view& name)
    {
        return;
    }
    common::result<float, exception> yaml_view::deserialize_float(std::string_view& name)
    {
        return;
    }
    common::result<double, exception> yaml_view::deserialize_double(std::string_view& name)
    {
        return;
    }
    bool yaml_view::deserialize_bool(std::string_view& name)
    {
        return;
    }
    common::result<std::string, exception> yaml_view::deserialize_string(std::string_view& name)
    {
        return;
    }
    common::result<id_type, exception> yaml_view::deserialize_id_type(std::string_view& name)
    {
        return;
    }
#pragma endregion
}
