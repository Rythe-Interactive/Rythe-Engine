#pragma once
#include <core/core.hpp>

#include <nlohmann/json.hpp>

#include <iostream>

namespace legion::core::serialization
{
    using json = nlohmann::ordered_json;

    struct serializer_view
    {
        serializer_view() = default;
        virtual ~serializer_view() = default;

        template<typename Type>
        bool serialize(std::string name, Type&& value)
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

        void start_object(id_type typeId);
        void end_object();

        virtual void serialize_int(std::string name, int serializable) = 0;
        virtual void serialize_float(std::string name, float serializable) = 0;
        virtual void serialize_double(std::string name, double serializable) = 0;
        virtual void serialize_bool(std::string name, bool serializable) = 0;
        virtual void serialize_string(std::string name, const std::string_view& serializable) = 0;
        virtual void serialize_id_type(std::string name, id_type serializable) = 0;

        virtual void write_result(fs::view& file) = 0;

        virtual void load_file(fs::view& file) = 0;

        virtual id_type deserialize_object_id(std::string name) = 0;
        virtual common::result<int, fs_error> deserialize_int(std::string name) = 0; // these should return a result type with errors
        virtual common::result<float, fs_error> deserialize_float(std::string name) = 0;
        virtual common::result<double, fs_error> deserialize_double(std::string name) = 0;
        virtual common::result<bool, fs_error> deserialize_bool(std::string name) = 0;
        virtual common::result<std::string, fs_error> deserialize_string(std::string name) = 0;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string name) = 0;
    };

    struct json_view : serializer_view
    {
        json data;
        std::string_view filePath;

        json_view() = default;
        json_view(std::string_view& filePath, const json&& j = json()) : filePath(filePath), data(j)
        {
            if (!j.is_null())
            {
                std::ifstream file(filePath);
                data = json::parse(file);
                if (data.is_discarded())
                    data = json();
            }
        }
        ~json_view() = default;

        virtual void serialize_int(std::string name, int serializable) override
        {
            data[name] = serializable;
        }
        virtual void serialize_float(std::string name, float serializable) override
        {
            data[name] = serializable;
        }
        virtual void serialize_double(std::string name, double serializable) override
        {
            data[name] = serializable;
        }
        virtual void serialize_bool(std::string name, bool serializable) override
        {
            data[name] = serializable;
        }
        virtual void serialize_string(std::string name, const std::string_view& serializable) override
        {
            data[name] = serializable;
        }
        virtual void serialize_id_type(std::string name, id_type serializable) override
        {
            data[name] = serializable;
        }

        virtual common::result<int, fs_error> deserialize_int(std::string name) override
        {
            return data[name];
        }
        virtual common::result<float, fs_error> deserialize_float(std::string name) override
        {
            return data[name].get<float>();
        }
        virtual common::result<double, fs_error> deserialize_double(std::string name) override
        {
            return data[name].get<double>();
        }
        virtual common::result<bool, fs_error> deserialize_bool(std::string name) override
        {
            return data[name].get<bool>();
        }
        virtual common::result<std::string, fs_error> deserialize_string(std::string name) override
        {
            return data[name].get<std::string>();
        }
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string name) override
        {
            auto id = data[name];
            return (id_type)id;
        }
    };

    struct bson_view : serializer_view
    {

        virtual void serialize_int(std::string name, int serializable) override
        {
        }
        virtual void serialize_float(std::string name, float serializable) override
        {

        }
        virtual void serialize_double(std::string name, double serializable) override
        {

        }
        virtual void serialize_bool(std::string name, bool serializable) override
        {

        }
        virtual void serialize_string(std::string name, const std::string_view& serializable) override
        {

        }
        virtual void serialize_id_type(std::string name, id_type serializable) override
        {

        }

        virtual common::result<int, fs_error> deserialize_int(std::string name) override
        {
            return;
        }
        virtual common::result<float, fs_error> deserialize_float(std::string name) override
        {
            return;
        }
        virtual common::result<double, fs_error> deserialize_double(std::string name) override
        {
            return;
        }
        virtual common::result<bool, fs_error> deserialize_bool(std::string name) override
        {
            return;
        }
        virtual common::result<std::string, fs_error> deserialize_string(std::string name) override
        {
            return;
        }
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string name) override
        {
            return;
        }
    };

    struct yaml_view : serializer_view
    {

        virtual void serialize_int(std::string name, int serializable) override
        {

        }
        virtual void serialize_float(std::string name, float serializable) override
        {

        }
        virtual void serialize_double(std::string name, double serializable) override
        {

        }
        virtual void serialize_bool(std::string name, bool serializable) override
        {

        }
        virtual void serialize_string(std::string name, const std::string_view& serializable) override
        {

        }
        virtual void serialize_id_type(std::string name, id_type serializable) override
        {

        }

        virtual common::result<int, fs_error> deserialize_int(std::string name) override
        {
            return;
        }
        virtual common::result<float, fs_error> deserialize_float(std::string name) override
        {
            return;
        }
        virtual common::result<double, fs_error> deserialize_double(std::string name) override
        {
            return;
        }
        virtual common::result<bool, fs_error> deserialize_bool(std::string name) override
        {
            return;
        }
        virtual common::result<std::string, fs_error> deserialize_string(std::string name) override
        {
            return;
        }
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string name) override
        {
            return;
        }
    };
}

