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
    void json_view::serialize_int(std::string_view& name, int serializable)
    {
        data[object_name][name.data()] = serializable;
    }
    void json_view::serialize_float(std::string_view& name, float serializable)
    {
        data[object_name][name.data()] = serializable;
    }
    void json_view::serialize_double(std::string_view& name, double serializable)
    {
        data[object_name][name.data()] = serializable;
    }
    void json_view::serialize_bool(std::string_view& name, bool serializable)
    {
        data[object_name][name.data()] = serializable;
    }
    void json_view::serialize_string(std::string_view& name, const std::string_view& serializable)
    {
        data[object_name][name.data()] = serializable;
    }
    void json_view::serialize_id_type(std::string_view& name, id_type serializable)
    {
        data[object_name][name.data()] = serializable;
    }

    common::result<int, fs_error> json_view::deserialize_int(std::string_view& name)
    {
        return data[object_name][name.data()].get<int>();
    }
    common::result<float, fs_error> json_view::deserialize_float(std::string_view& name)
    {
        return data[object_name][name.data()].get<float>();
    }
    common::result<double, fs_error> json_view::deserialize_double(std::string_view& name)
    {
        return data[object_name][name.data()].get<double>();
    }
    bool json_view::deserialize_bool(std::string_view& name)
    {
        return data[object_name][name.data()].get<bool>();
    }
    common::result<std::string, fs_error> json_view::deserialize_string(std::string_view& name)
    {
        return data[object_name][name.data()].get<std::string>();
    }
    common::result<id_type, fs_error> json_view::deserialize_id_type(std::string_view& name)
    {
        auto id = data[object_name][name.data()].get<int>();
        return (id_type)id;
    }
#pragma endregion

#pragma region bson_view
    void bson_view::serialize_int(std::string_view& name, int serializable)
    {

    }
    void bson_view::serialize_float(std::string_view& name, float serializable)
    {

    }
    void bson_view::serialize_double(std::string_view& name, double serializable)
    {

    }
    void bson_view::serialize_bool(std::string_view& name, bool serializable)
    {

    }
    void bson_view::serialize_string(std::string_view& name, const std::string_view& serializable)
    {

    }
    void bson_view::serialize_id_type(std::string_view& name, id_type serializable)
    {

    }

    common::result<int, fs_error> bson_view::deserialize_int(std::string_view& name)
    {
        return;
    }
    common::result<float, fs_error> bson_view::deserialize_float(std::string_view& name)
    {
        return;
    }
    common::result<double, fs_error> bson_view::deserialize_double(std::string_view& name)
    {
        return;
    }
    bool bson_view::deserialize_bool(std::string_view& name)
    {
        return;
    }
    common::result<std::string, fs_error> bson_view::deserialize_string(std::string_view& name)
    {
        return;
    }
    common::result<id_type, fs_error> bson_view::deserialize_id_type(std::string_view& name)
    {
        return;
    }
#pragma endregion

#pragma region yaml_view
    void yaml_view::serialize_int(std::string_view& name, int serializable)
    {

    }
    void yaml_view::serialize_float(std::string_view& name, float serializable)
    {

    }
    void yaml_view::serialize_double(std::string_view& name, double serializable)
    {

    }
    void yaml_view::serialize_bool(std::string_view& name, bool serializable)
    {

    }
    void yaml_view::serialize_string(std::string_view& name, const std::string_view& serializable)
    {

    }
    void yaml_view::serialize_id_type(std::string_view& name, id_type serializable)
    {

    }

    common::result<int, fs_error> yaml_view::deserialize_int(std::string_view& name)
    {
        return;
    }
    common::result<float, fs_error> yaml_view::deserialize_float(std::string_view& name)
    {
        return;
    }
    common::result<double, fs_error> yaml_view::deserialize_double(std::string_view& name)
    {
        return;
    }
    bool yaml_view::deserialize_bool(std::string_view& name)
    {
        return;
    }
    common::result<std::string, fs_error> yaml_view::deserialize_string(std::string_view& name)
    {
        return;
    }
    common::result<id_type, fs_error> yaml_view::deserialize_id_type(std::string_view& name)
    {
        return;
    }
#pragma endregion
}
