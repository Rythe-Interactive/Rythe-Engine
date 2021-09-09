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
    void json_view::serialize_int(std::string name, int serializable)
    {
        data[name] = serializable;
    }
    void json_view::serialize_float(std::string name, float serializable)
    {
        data[name] = serializable;
    }
    void json_view::serialize_double(std::string name, double serializable)
    {
        data[name] = serializable;
    }
    void json_view::serialize_bool(std::string name, bool serializable)
    {
        data[name] = serializable;
    }
    void json_view::serialize_string(std::string name, const std::string_view& serializable)
    {
        data[name] = serializable;
    }
    void json_view::serialize_id_type(std::string name, id_type serializable)
    {
        data[name] = serializable;
    }

    common::result<int, fs_error> json_view::deserialize_int(std::string& name)
    {
        return data[name].get<int>();
    }
    common::result<float, fs_error> json_view::deserialize_float(std::string& name)
    {
        return data[name].get<float>();
    }
    common::result<double, fs_error> json_view::deserialize_double(std::string& name)
    {
        return data[name].get<double>();
    }
    bool json_view::deserialize_bool(std::string& name)
    {
        return data[name].get<bool>();
    }
    common::result<std::string, fs_error> json_view::deserialize_string(std::string& name)
    {
        return data[name].get<std::string>();
    }
    common::result<id_type, fs_error> json_view::deserialize_id_type(std::string& name)
    {
        auto id = data[name].get<int>();
        return (id_type)id;
    }
#pragma endregion

#pragma region bson_view
    void bson_view::serialize_int(std::string name, int serializable)
    {

    }
    void bson_view::serialize_float(std::string name, float serializable)
    {

    }
    void bson_view::serialize_double(std::string name, double serializable)
    {

    }
    void bson_view::serialize_bool(std::string name, bool serializable)
    {

    }
    void bson_view::serialize_string(std::string name, const std::string_view& serializable)
    {

    }
    void bson_view::serialize_id_type(std::string name, id_type serializable)
    {

    }

    common::result<int, fs_error> bson_view::deserialize_int(std::string& name)
    {
        return;
    }
    common::result<float, fs_error> bson_view::deserialize_float(std::string& name)
    {
        return;
    }
    common::result<double, fs_error> bson_view::deserialize_double(std::string& name)
    {
        return;
    }
    bool bson_view::deserialize_bool(std::string& name)
    {
        return;
    }
    common::result<std::string, fs_error> bson_view::deserialize_string(std::string& name)
    {
        return;
    }
    common::result<id_type, fs_error> bson_view::deserialize_id_type(std::string& name)
    {
        return;
    }
#pragma endregion

#pragma region yaml_view
    void yaml_view::serialize_int(std::string name, int serializable)
    {

    }
    void yaml_view::serialize_float(std::string name, float serializable)
    {

    }
    void yaml_view::serialize_double(std::string name, double serializable)
    {

    }
    void yaml_view::serialize_bool(std::string name, bool serializable)
    {

    }
    void yaml_view::serialize_string(std::string name, const std::string_view& serializable)
    {

    }
    void yaml_view::serialize_id_type(std::string name, id_type serializable)
    {

    }

    common::result<int, fs_error> yaml_view::deserialize_int(std::string& name)
    {
        return;
    }
    common::result<float, fs_error> yaml_view::deserialize_float(std::string& name)
    {
        return;
    }
    common::result<double, fs_error> yaml_view::deserialize_double(std::string& name)
    {
        return;
    }
    bool yaml_view::deserialize_bool(std::string& name)
    {
        return;
    }
    common::result<std::string, fs_error> yaml_view::deserialize_string(std::string& name)
    {
        return;
    }
    common::result<id_type, fs_error> yaml_view::deserialize_id_type(std::string& name)
    {
        return;
    }
#pragma endregion
}
