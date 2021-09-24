#include <core/serialization/serializer_views/json.hpp>
#pragma once

namespace legion::core::serialization
{
    inline void json::serialize_int(std::string& name, int serializable)
    {
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, serializable);
    }

    inline void json::serialize_float(std::string& name, float serializable)
    {
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, serializable);
    }

    inline void json::serialize_double(std::string& name, double serializable)
    {
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, serializable);
    }

    inline void json::serialize_bool(std::string& name, bool serializable)
    {
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, serializable);
    }

    inline void json::serialize_string(std::string& name, const std::string_view& serializable)
    {
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, serializable);
    }

    inline void json::serialize_id_type(std::string& name, id_type serializable)
    {
        int id = (int)serializable;
        auto key = write_queue.top().begin().key();
        write_queue.top()[key].emplace(name, id);
    }


    inline common::result<int, fs_error> json::deserialize_int(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<float, fs_error> json::deserialize_float(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<double, fs_error> json::deserialize_double(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<void, fs_error>  json::deserialize_bool(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<std::string, fs_error> json::deserialize_string(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<id_type, fs_error> json::deserialize_id_type(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }


    inline void json::start_object()
    {
        write_queue.emplace();
    }

    inline void json::start_object(std::string name)
    {
        nlohmann::ordered_json j;
        j[name] = {  };
        write_queue.emplace(j);
    }

    inline void json::end_object()
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

    inline void json::start_container(std::string name)
    {
        nlohmann::ordered_json j;
        j[name] = { nlohmann::ordered_json::array() };
        write_queue.emplace(j);
    }


    inline void json::end_container()
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

    inline common::result<void, fs_error> json::write(fs::view& file)
    {
        return file.set(fs::basic_resource(root.dump()));
    }

    inline common::result<void, fs_error> json::read(fs::view& file)
    {
        return legion_fs_error("not yet implemented");
    }

    inline common::result<void, fs_error> json::read(byte_vec data)
    {
        return legion_fs_error("not yet implemented");
    }

    inline common::result<void, fs_error> json::read(byte_vec::iterator begin, byte_vec::iterator end)
    {
        return legion_fs_error("not yet implemented");
    }
}
