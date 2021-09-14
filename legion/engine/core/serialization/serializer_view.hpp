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

        virtual void start_object(std::string name) = 0;
        virtual void end_object() = 0;

        virtual void start_container(std::string name) = 0;
        virtual void end_container() = 0;

        template<typename Type>
        bool serialize(std::string_view name, Type&& value)
        {
            using raw_type = std::decay_t<Type>;

            if constexpr (std::is_same_v<raw_type, int>)
            {
                serialize_int(name, std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, float>)
            {
                serialize_float(name, std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, double>)
            {
                serialize_double(name, std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, bool>)
            {
                serialize_bool(name, std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, std::string>)
            {
                serialize_string(name, std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, id_type>)
            {
                serialize_id_type(name, std::move(value));
                return true;
            }
            return false;
        }

        virtual void serialize_int(std::string_view& name, int serializable) = 0;
        virtual void serialize_float(std::string_view& name, float serializable) = 0;
        virtual void serialize_double(std::string_view& name, double serializable) = 0;
        virtual void serialize_bool(std::string_view& name, bool serializable) = 0;
        virtual void serialize_string(std::string_view& name, const std::string_view& serializable) = 0;
        virtual void serialize_id_type(std::string_view& name, id_type serializable) = 0;

        //virtual void write_result(fs::view& file) = 0;

        //virtual void load_file(fs::view& file) = 0;

        virtual common::result<int, exception> deserialize_int(std::string_view& name) = 0;
        virtual common::result<float, exception> deserialize_float(std::string_view& name) = 0;
        virtual common::result<double, exception> deserialize_double(std::string_view& name) = 0;
        virtual bool deserialize_bool(std::string_view& name) = 0;
        virtual common::result<std::string, exception> deserialize_string(std::string_view& name) = 0;
        virtual common::result<id_type, exception> deserialize_id_type(std::string_view& name) = 0;
    };

    struct json_view : serializer_view
    {
        std::string data;

        json_view() = default;
        ~json_view() = default;

        virtual void start_object(std::string name) override
        {
            if (name.size() > 0)
            {
                data.append(name);
                data.append(":");
            }
            data.append("{");
        }
        virtual void end_object() override
        {
            data.pop_back();
            data.append("},");
        }

        virtual void start_container(std::string name) override
        {
            if (name.size() > 0)
            {
                data.append(name);
                data.append(":");
            }
            data.append("[");
        }
        virtual void end_container() override
        {
            if (data[data.size() - 1] == ',')
                data.pop_back();
            data.append("],");
        }

        template<typename Type>
        bool serialize(std::string_view name, Type&& value)
        {
            using raw_type = std::decay_t<Type>;

            if constexpr (std::is_same_v<raw_type, int>)
            {
                serialize_int(name, std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, float>)
            {
                serialize_float(name, std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, double>)
            {
                serialize_double(name, std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, bool>)
            {
                serialize_bool(name, std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, std::string>)
            {
                serialize_string(name, std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, id_type>)
            {
                serialize_id_type(name, std::move(value));
                return true;
            }
            return false;
        }

        virtual void serialize_int(std::string_view& name, int serializable) override;
        virtual void serialize_float(std::string_view& name, float serializable) override;
        virtual void serialize_double(std::string_view& name, double serializable) override;
        virtual void serialize_bool(std::string_view& name, bool serializable) override;
        virtual void serialize_string(std::string_view& name, const std::string_view& serializable) override;
        virtual void serialize_id_type(std::string_view& name, id_type serializable) override;

        virtual common::result<int, exception> deserialize_int(std::string_view& name) override;
        virtual common::result<float, exception> deserialize_float(std::string_view& name) override;
        virtual common::result<double, exception> deserialize_double(std::string_view& name) override;
        virtual bool deserialize_bool(std::string_view& name) override;
        virtual common::result<std::string, exception> deserialize_string(std::string_view& name) override;
        virtual common::result<id_type, exception> deserialize_id_type(std::string_view& name) override;
    };

    struct bson_view : serializer_view
    {

        bson_view() = default;
        ~bson_view() = default;

        virtual void start_object(std::string name) override
        {

        }
        virtual void end_object() override
        {

        }

        virtual void start_container(std::string name) override
        {

        }
        virtual void end_container() override
        {

        }

        template<typename Type>
        bool serialize(std::string_view name, Type&& value)
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
                serialize_id_type(name, std::move(value));
                return true;
            }
            return false;
        }

        virtual void serialize_int(std::string_view& name, int serializable) override;
        virtual void serialize_float(std::string_view& name, float serializable) override;
        virtual void serialize_double(std::string_view& name, double serializable) override;
        virtual void serialize_bool(std::string_view& name, bool serializable) override;
        virtual void serialize_string(std::string_view& name, const std::string_view& serializable) override;
        virtual void serialize_id_type(std::string_view& name, id_type serializable) override;

        virtual common::result<int, exception> deserialize_int(std::string_view& name) override;
        virtual common::result<float, exception> deserialize_float(std::string_view& name) override;
        virtual common::result<double, exception> deserialize_double(std::string_view& name) override;
        virtual bool deserialize_bool(std::string_view& name) override;
        virtual common::result<std::string, exception> deserialize_string(std::string_view& name) override;
        virtual common::result<id_type, exception> deserialize_id_type(std::string_view& name) override;
    };

    struct yaml_view : serializer_view
    {

        yaml_view() = default;
        ~yaml_view() = default;

        virtual void start_object(std::string name) override
        {

        }
        virtual void end_object() override
        {

        }

        virtual void start_container(std::string name) override
        {

        }
        virtual void end_container() override
        {

        }

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
                serialize_id_type(name, std::move(value));
                return true;
            }
            return false;
        }

        virtual void serialize_int(std::string_view& name, int serializable) override;
        virtual void serialize_float(std::string_view& name, float serializable) override;
        virtual void serialize_double(std::string_view& name, double serializable) override;
        virtual void serialize_bool(std::string_view& name, bool serializable) override;
        virtual void serialize_string(std::string_view& name, const std::string_view& serializable) override;
        virtual void serialize_id_type(std::string_view& name, id_type serializable) override;

        virtual common::result<int, exception> deserialize_int(std::string_view& name) override;
        virtual common::result<float, exception> deserialize_float(std::string_view& name) override;
        virtual common::result<double, exception> deserialize_double(std::string_view& name) override;
        virtual bool deserialize_bool(std::string_view& name) override;
        virtual common::result<std::string, exception> deserialize_string(std::string_view& name) override;
        virtual common::result<id_type, exception> deserialize_id_type(std::string_view& name) override;
    };
}

#include <core/serialization/serializer_view.inl>

