#include <core/serialization/serializer_view.hpp>
#pragma once

namespace legion::core::serialization
{
#pragma region serializer_view
    //template<typename Type>
    //bool serializer_view::serialize(std::string& name, Type&& value)
    //{

    //}
#pragma endregion

#pragma region json_view
    void json_view::serialize_int(std::string& name, int serializable)
    {
        data.append(name);
        data.append(":");
        data.append(std::to_string(serializable));
        data.append(",");
    }
    void json_view::serialize_float(std::string& name, float serializable)
    {
        data.append(name);
        data.append(":");
        data.append(std::to_string(serializable));
        data.append(",");
    }
    void json_view::serialize_double(std::string& name, double serializable)
    {
        data.append(name);
        data.append(":");
        data.append(std::to_string(serializable));
        data.append(",");
    }
    void json_view::serialize_bool(std::string& name, bool serializable)
    {
        data.append(name);
        data.append(":");
        data.append(std::to_string(serializable));
        data.append(",");
    }
    void json_view::serialize_string(std::string& name, const std::string_view& serializable)
    {
        data.append(name);
        data.append(":");
        data.append(serializable);
        data.append(",");
    }
    void json_view::serialize_id_type(std::string& name, id_type serializable)
    {
        data.append(name);
        data.append(":");
        data.append(std::to_string(serializable));
        data.append(",");
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
