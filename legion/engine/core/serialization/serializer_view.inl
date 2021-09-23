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

    template<typename Type>
    inline common::result<Type> serializer_view::deserialize(std::string_view& name)
    {
        return false;
    }
#pragma endregion

#pragma region json_view
    void json_view::serialize_int(std::string& name, int serializable)
    {
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, serializable);
    }

    void json_view::serialize_float(std::string& name, float serializable)
    {
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, serializable);
    }

    void json_view::serialize_double(std::string& name, double serializable)
    {
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, serializable);
    }

    void json_view::serialize_bool(std::string& name, bool serializable)
    {
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, serializable);
    }

    void json_view::serialize_string(std::string& name, const std::string_view& serializable)
    {
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, serializable);
    }

    void json_view::serialize_id_type(std::string& name, id_type serializable)
    {
        int id = (int)serializable;
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, id);
    }


    common::result<int, fs_error> json_view::deserialize_int(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    common::result<float, fs_error> json_view::deserialize_float(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    common::result<double, fs_error> json_view::deserialize_double(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    bool json_view::deserialize_bool(std::string_view& name)
    {
        return false;
    }

    common::result<std::string, fs_error> json_view::deserialize_string(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    common::result<id_type, fs_error> json_view::deserialize_id_type(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }


    inline void json_view::start_object()
    {
        write_queue.emplace();
    }

    inline void json_view::start_object(std::string name)
    {
        json j;
        j[name] = {  };
        write_queue.emplace(j);
    }

    inline void json_view::end_object()
    {
        if (write_queue.empty())
            return;

        auto top_object = write_queue.top();
        auto current_val = top_object.begin().value();
        auto current_key = top_object.begin().key();

        if (!current_val.is_object())
            return;

        write_queue.pop();
        if (write_queue.empty())
            root.emplace(current_key, std::move(current_val));
        else
        {
            auto& _next = write_queue.top();
            auto& next = _next.begin().value();

            if (next.is_array())
                next.emplace_back(top_object);
            else if (next.is_object())
                next.emplace(current_key, std::move(current_val));
        }
    }

    inline void json_view::start_container(std::string name)
    {
        json j;
        j[name] = { json::array() };
        write_queue.emplace(j);
    }


    inline void json_view::end_container()
    {
        auto top_object = write_queue.top();
        auto current_val = top_object.begin().value();
        auto current_key = top_object.begin().key();

        if (!current_val.is_array())
            return;

        write_queue.pop();
        if (write_queue.empty())
            root.emplace(current_key, std::move(current_val));
        else if (!current_val.is_null())
        {
            auto& _next = write_queue.top();
            auto& next = _next.begin().value();

            if (next.is_array())
                next.emplace_back(top_object);
            else if (next.is_object())
                next.emplace(current_key, std::move(current_val));
        }
    }

    inline common::result<void, fs_error> json_view::write()
    {
        return file.set(fs::basic_resource(root.dump()));
    }

    inline common::result<void, fs_error> json_view::load(fs::view& file)
    {
        return legion_fs_error("not implemented");
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

    common::result<int, fs_error> bson_view::deserialize_int(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    common::result<float, fs_error> bson_view::deserialize_float(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    common::result<double, fs_error> bson_view::deserialize_double(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    bool bson_view::deserialize_bool(std::string_view& name)
    {
        return false;
    }

    common::result<std::string, fs_error> bson_view::deserialize_string(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    common::result<id_type, fs_error> bson_view::deserialize_id_type(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline void bson_view::start_object()
    {

    }

    inline void bson_view::start_object(std::string name)
    {

    }

    inline void bson_view::end_object()
    {

    }

    inline void bson_view::start_container(std::string name)
    {
    }

    inline void bson_view::end_container()
    {
    }

    inline common::result<void, fs_error> bson_view::write()
    {
        return legion_fs_error("not yet implemented");
    }

    inline common::result<void, fs_error> bson_view::load(fs::view& file)
    {
        return legion_fs_error("not yet implemented");
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

    common::result<int, fs_error> yaml_view::deserialize_int(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }
    common::result<float, fs_error> yaml_view::deserialize_float(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }
    common::result<double, fs_error> yaml_view::deserialize_double(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }
    bool yaml_view::deserialize_bool(std::string_view& name)
    {
        return false;
    }

    common::result<std::string, fs_error> yaml_view::deserialize_string(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    common::result<id_type, fs_error> yaml_view::deserialize_id_type(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline void yaml_view::start_object()
    {
    }

    inline void yaml_view::start_object(std::string name)
    {
    }

    inline void yaml_view::end_object()
    {
    }

    inline void yaml_view::start_container(std::string name)
    {
    }

    inline void yaml_view::end_container()
    {
    }

    inline common::result<void, fs_error> yaml_view::write()
    {
        return legion_fs_error("Not yet implemented");
    }
    inline common::result<void, fs_error> yaml_view::load(fs::view& file)
    {
        return legion_fs_error("Not yet implemented");
    }
#pragma endregion
}
