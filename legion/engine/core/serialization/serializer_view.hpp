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
        serializer_view(serializer_view& s_view) {};
        serializer_view(std::string name, serializer_view& s_view) {};
        virtual ~serializer_view() = default;

        virtual void add_view(serializer_view& view) = 0;

        template<typename Type>
        bool serialize(std::string name, Type&& value)
        {
            using raw_type = std::decay_t<Type>;

            if constexpr (std::is_same_v<raw_type, int>)
            {
                serialize_int(name,std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, float>)
            {
                serialize_float(name,std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, double>) 
            {
                serialize_double(name, std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, bool>)
            {
                serialize_bool(name,std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, std::string>)
            {
                serialize_string(name,std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, id_type>)
            {
                serialize_id_type(name,std::move(value));
                return true;
            }
            return false;
        }

        virtual void serialize_int(std::string name, int serializable) = 0;
        virtual void serialize_float(std::string name,float serializable) = 0;
        virtual void serialize_double(std::string name, double serializable) = 0;
        virtual void serialize_bool(std::string name, bool serializable) = 0;
        virtual void serialize_string(std::string name, const std::string_view& serializable) = 0;
        virtual void serialize_id_type(std::string name, id_type serializable) = 0;

        //virtual void write_result(fs::view& file) = 0;

        //virtual void load_file(fs::view& file) = 0;

        //virtual id_type deserialize_object_id(std::string& name) = 0;
        virtual common::result<int, fs_error> deserialize_int(std::string& name) = 0; // these should return a result type with errors
        virtual common::result<float, fs_error> deserialize_float(std::string& name) = 0;
        virtual common::result<double, fs_error> deserialize_double(std::string& name) = 0;
        virtual bool deserialize_bool(std::string& name) = 0;
        virtual common::result<std::string, fs_error> deserialize_string(std::string& name) = 0;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string& name) = 0;
    };

    struct json_view : serializer_view
    {
        json data;
        std::string_view filePath;

        json_view() = default;
        json_view(std::string_view& filePath, const json&& j = json()) : filePath(filePath), data(j) { }
        ~json_view() = default;

        virtual void add_view(serializer_view& view) override;

        template<typename Type>
        bool serialize(std::string name, Type&& value)
        {
            using raw_type = std::decay_t<Type>;
            if constexpr (std::is_same_v<raw_type, int>)
            {
                serialize_int(name,value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, float>)
            {
                serialize_float(name,value);
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

        virtual void serialize_int(std::string name, int serializable) override;
        virtual void serialize_float(std::string name, float serializable) override;
        virtual void serialize_double(std::string name, double serializable) override;
        virtual void serialize_bool(std::string name, bool serializable) override;
        virtual void serialize_string(std::string name, const std::string_view& serializable) override;
        virtual void serialize_id_type(std::string name, id_type serializable) override;

        virtual common::result<int, fs_error> deserialize_int(std::string& name) override;
        virtual common::result<float, fs_error> deserialize_float(std::string& name) override;
        virtual common::result<double, fs_error> deserialize_double(std::string& name) override;
        virtual bool deserialize_bool(std::string& name) override;
        virtual common::result<std::string, fs_error> deserialize_string(std::string& name) override;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string& name) override;
    };

    struct bson_view : serializer_view
    {
        std::string_view filePath;

        bson_view() = default;
        ~bson_view() = default;

        virtual void add_view(serializer_view& view) override;

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

        virtual void serialize_int(std::string name, int serializable) override;
        virtual void serialize_float(std::string name, float serializable) override;
        virtual void serialize_double(std::string name, double serializable) override;
        virtual void serialize_bool(std::string name, bool serializable) override;
        virtual void serialize_string(std::string name, const std::string_view& serializable) override;
        virtual void serialize_id_type(std::string name, id_type serializable) override;

        virtual common::result<int, fs_error> deserialize_int(std::string& name) override;
        virtual common::result<float, fs_error> deserialize_float(std::string& name) override;
        virtual common::result<double, fs_error> deserialize_double(std::string& name) override;
        virtual bool deserialize_bool(std::string& name) override;
        virtual common::result<std::string, fs_error> deserialize_string(std::string& name) override;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string& name) override;
    };

    struct yaml_view : serializer_view
    {
        std::string_view filePath;

        yaml_view() = default;
        ~yaml_view() = default;

        virtual void add_view(serializer_view& view) override;

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

        virtual void serialize_int(std::string name, int serializable) override;
        virtual void serialize_float(std::string name, float serializable) override;
        virtual void serialize_double(std::string name, double serializable) override;
        virtual void serialize_bool(std::string name, bool serializable) override;
        virtual void serialize_string(std::string name, const std::string_view& serializable) override;
        virtual void serialize_id_type(std::string name, id_type serializable) override;

        virtual common::result<int, fs_error> deserialize_int(std::string& name) override;
        virtual common::result<float, fs_error> deserialize_float(std::string& name) override;
        virtual common::result<double, fs_error> deserialize_double(std::string& name) override;
        virtual bool deserialize_bool(std::string& name) override;
        virtual common::result<std::string, fs_error> deserialize_string(std::string& name) override;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string& name) override;
    };
}

#include <core/serialization/serializer_view.inl>

