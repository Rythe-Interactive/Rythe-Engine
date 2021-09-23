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
    inline void json_view::start_object()
    {
        log::debug("object start");
        log::debug("element");

        json j;
        write_queue.emplace(j);
    }

    inline void json_view::start_object(std::string name)
    {
        log::debug("object start");
        log::debug(name);

        json j;
        j.emplace(name,json::object());
        log::debug(j.dump());
        write_queue.emplace(j);
    }

    inline void json_view::end_object()
    {
        if (write_queue.empty())
        {
            log::debug("Queue is empty");
            return;
        }
        auto cur = write_queue.top();

        if (!cur.is_object())
        {
            log::debug("Top of queue is not an object");
            return;
        }

        auto key = cur.begin().key();
        write_queue.pop();
        if (write_queue.empty())
        {
            cur.erase(cur.begin());
            root.emplace(key,std::move(cur));
        }
        else
        {
            log::debug(key);
            auto& next = write_queue.top();

            if (next.is_array())
            {
                log::debug("array detected, adding to array");
                next.emplace_back(cur);
            }
            else if (next.is_object())
            {
                log::debug("adding value");
                cur.erase(cur.begin());
                next.emplace(key, std::move(cur));
            }
        }

        log::debug("object end");
        log::debug("Stack size");
        log::debug(write_queue.size());
    }

    inline void json_view::start_container(std::string name)
    {
        log::debug("array start");
        log::debug(name);

        json j;
        j.emplace(name, json::array());
        log::debug(j.dump());
        write_queue.emplace(j);
    }

    inline void json_view::end_container()
    {
        if (write_queue.empty())
        {
            log::debug("Queue is empty");
            return;
        }
        auto cur = write_queue.top();

        if (!cur.is_object())
        {
            log::debug("Top of queue is not an object");
            return;
        }

        auto key = cur.begin().key();
        write_queue.pop();
        if (write_queue.empty())
        {
            cur.erase(cur.begin());
            root.emplace(key, std::move(cur));
        }
        else
        {
            log::debug(key);
            auto& next = write_queue.top();

            if (next.is_array())
            {
                log::debug("array detected, adding to array");
                next.emplace_back(cur);
            }
            else if (next.is_object())
            {
                log::debug("adding value");
                cur.erase(cur.begin());
                next.emplace(key, std::move(cur));
            }
        }
    }

    void json_view::serialize_int(std::string& name, int serializable)
    {
        write_queue.top().emplace(name, serializable);
    }

    void json_view::serialize_float(std::string& name, float serializable)
    {
        write_queue.top().emplace(name, serializable);
    }

    void json_view::serialize_double(std::string& name, double serializable)
    {
        write_queue.top().emplace(name, serializable);
    }

    void json_view::serialize_bool(std::string& name, bool serializable)
    {
        write_queue.top().emplace(name, serializable);
    }

    void json_view::serialize_string(std::string& name, const std::string_view& serializable)
    {
        write_queue.top().emplace(name, serializable);
    }

    void json_view::serialize_id_type(std::string& name, id_type serializable)
    {
        int id = (int)serializable;
        write_queue.top().emplace(name, serializable);
    }

    inline common::result<void, fs_error> json_view::write()
    {
        log::debug(root.dump());
        return file.set(fs::basic_resource(root.dump()));
    }

    inline common::result<void, fs_error> json_view::load(fs::view& file)
    {
        return legion_fs_error("not implemented");
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
