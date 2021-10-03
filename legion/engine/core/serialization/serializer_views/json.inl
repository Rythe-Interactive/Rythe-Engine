#include <core/serialization/serializer_views/json.hpp>
#pragma once

namespace legion::core::serialization
{
    inline void json::serialize_int(std::string& name, int serializable)
    {
        auto key = active_stack.top().begin().key();
        active_stack.top()[key].emplace(name, serializable);
    }

    inline void json::serialize_float(std::string& name, float serializable)
    {
        auto key = active_stack.top().begin().key();
        active_stack.top()[key].emplace(name, serializable);
    }

    inline void json::serialize_double(std::string& name, double serializable)
    {
        auto key = active_stack.top().begin().key();
        active_stack.top()[key].emplace(name, serializable);
    }

    inline void json::serialize_bool(std::string& name, bool serializable)
    {
        auto key = active_stack.top().begin().key();
        active_stack.top()[key].emplace(name, serializable);
    }

    inline void json::serialize_string(std::string& name, const std::string_view& serializable)
    {
        auto key = active_stack.top().begin().key();
        active_stack.top()[key].emplace(name, serializable);
    }

    inline void json::serialize_id_type(std::string& name, id_type serializable)
    {
        int id = (int)serializable;
        auto key = active_stack.top().begin().key();
        active_stack.top()[key].emplace(name, id);
    }


    inline common::result<int, fs_error> json::deserialize_int(std::string& name)
    {
        auto output = active_stack.top()[name].get<int>();
        active_stack.top().erase(name);
        if (active_stack.top().size() < 1)
            active_stack.pop();
        return output;
    }

    inline common::result<float, fs_error> json::deserialize_float(std::string& name)
    {
        auto output = active_stack.top()[name].get<float>();
        active_stack.top().erase(name);
        if (active_stack.top().size() < 1)
            active_stack.pop();
        return output;
    }

    inline common::result<double, fs_error> json::deserialize_double(std::string& name)
    {
        auto output = active_stack.top()[name].get<double>();
        active_stack.top().erase(name);
        if (active_stack.top().size() < 1)
            active_stack.pop();
        return output;
    }

    inline bool json::deserialize_bool(std::string& name)
    {
        auto output = active_stack.top()[name].get<bool>();
        active_stack.top().erase(name);
        if (active_stack.top().size() < 1)
            active_stack.pop();
        return output;
    }

    inline common::result<std::string, fs_error> json::deserialize_string(std::string& name)
    {
        auto output = active_stack.top()[name].get<std::string>();
        active_stack.top().erase(name);
        if (active_stack.top().size() < 1)
            active_stack.pop();
        return output;
    }

    inline common::result<id_type, fs_error> json::deserialize_id_type(std::string& name)
    {
        auto output = active_stack.top()[name].get<int>();
        active_stack.top().erase(name);
        if (active_stack.top().size() < 1)
            active_stack.pop();
        return output;
    }

    inline void json::start_object()
    {
        active_stack.emplace();
    }

    inline void json::start_object(std::string name)
    {
        nlohmann::ordered_json j;
        j[name] = {  };
        active_stack.emplace(j);
    }

    inline void json::end_object()
    {
        if (active_stack.empty())
            return;

        auto top_object = active_stack.top();
        auto current_val = top_object.begin().value();
        auto current_key = top_object.begin().key();

        if (!current_val.is_object())
            return;

        active_stack.pop();
        if (active_stack.empty())
            root.emplace(current_key, std::move(current_val));
        else
        {
            auto& _next = active_stack.top();
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
        active_stack.emplace(j);
    }

    inline void json::end_container()
    {
        auto top_object = active_stack.top();
        auto current_val = top_object.begin().value();
        auto current_key = top_object.begin().key();

        if (!current_val.is_array())
            return;

        active_stack.pop();
        if (active_stack.empty())
            root.emplace(current_key, std::move(current_val));
        else if (!current_val.is_null())
        {
            auto& _next = active_stack.top();
            auto& next = _next.begin().value();

            if (next.is_array())
                next.emplace_back(top_object);
            else if (next.is_object())
                next.emplace(current_key, std::move(current_val));
        }
    }

    inline void json::start_read()
    {
        if (active_stack.empty())
        {
            active_stack.push(root);
        }

        active_stack.push(active_stack.top().begin().value());
        log::debug("Beginning read: " + active_stack.top().dump());
    }

    inline void json::end_read()
    {
        if (active_stack.empty())
            return;

        active_stack.pop();
    }

    inline int json::start_read_array()
    {
        start_read();

        if (active_stack.top().is_array())
            return active_stack.top().size();
        else
            return 0;
    }

    inline void json::end_read_array()
    {
        if (active_stack.top().size() < 1)
            active_stack.pop();

        active_stack.top().erase(active_stack.top().begin().key());
    }

    inline std::string json::get_key()
    {
        auto key = active_stack.top()[0].begin().key();
        auto val = active_stack.top()[0][key];
        active_stack.top().erase(0);
        active_stack.push(val);

        if (active_stack.top().size() < 1)
            active_stack.pop();

        return key;
    }

    inline common::result<void, fs_error> json::write(fs::view& file)
    {
        return file.set(fs::basic_resource(root.dump()));
    }

    inline common::result<void, fs_error> json::read(fs::view& file)
    {
        auto result = file.get();
        if (result.valid())
            log::debug(result.value().to_string());
        else
            log::debug(result.error().what());
        root = nlohmann::ordered_json::parse(file.get().value().to_string());
        return common::success;
    }

    inline common::result<void, fs_error> json::read(byte_vec data)
    {
        root = nlohmann::ordered_json::parse(data);
        return common::success;
    }

    inline common::result<void, fs_error> json::read(byte_vec::iterator begin, byte_vec::iterator end)
    {
        return legion_fs_error("not yet implemented");
    }
}
