#pragma once
#include <core/core.hpp>

#include <nlohmann/json.hpp>

#include <iostream>

namespace legion::core::serialization
{
    using json = nlohmann::ordered_json;

    struct serializer_view
    {
        std::string val_name;

        virtual ~serializer_view() = default;

        //virtual void start_object(std::string& name, id_type typeId) = 0;
        //virtual void end_object() = 0;

        template<typename Type>
        bool serialize(std::string& name, Type&& value)
        {
            val_name = name;
            using raw_type = std::decay_t<Type>;

            if constexpr (std::is_same_v<raw_type, int>)
            {
                serialize_int(std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, float>)
            {
                serialize_float(std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, double>) 
            {
                serialize_double(std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, bool>)
            {
                serialize_bool(std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, std::string>)
            {
                serialize_string(std::move(value));
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, id_type>)
            {
                serialize_id_type(std::move(value));
                return true;
            }
            return false;
        }

        virtual void serialize_int(int serializable) = 0;
        virtual void serialize_float( float serializable) = 0;
        virtual void serialize_double(double serializable) = 0;
        virtual void serialize_bool(bool serializable) = 0;
        virtual void serialize_string(const std::string_view& serializable) = 0;
        virtual void serialize_id_type(id_type serializable) = 0;

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
        std::string val_name;

        json_view() = default;
        json_view(std::string_view& filePath, const json&& j = json()) : filePath(filePath), data(j) { }
        ~json_view() = default;

        //void start_object(std::string& name, id_type& typeID)
        //{
        //    
        //}
        //void end_object()
        //{

        //}


        template<typename Type>
        bool serialize(std::string& name, Type&& value)
        {
            using raw_type = std::decay_t<Type>;
            data.
            if constexpr (std::is_same_v<raw_type, int>)
            {
                serialize_int(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, float>)
            {
                serialize_float(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, double>)
            {
                serialize_double(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, bool>)
            {
                serialize_bool(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, std::string>)
            {
                serialize_string(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, id_type>)
            {
                serialize_id_type(std::move(value));
                return true;
            }
            return false;
        }

        virtual void serialize_int(int serializable) override;
        virtual void serialize_float(float serializable) override;
        virtual void serialize_double(double serializable) override;
        virtual void serialize_bool(bool serializable) override;
        virtual void serialize_string(const std::string_view& serializable) override;
        virtual void serialize_id_type(id_type serializable) override;

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
        std::string val_name;

        bson_view() = default;
        ~bson_view() = default;

        //void start_object(std::string& name, id_type& typeID)
        //{

        //}
        //void end_object()
        //{

        //}

        template<typename Type>
        bool serialize(std::string& name, Type&& value)
        {
            val_name = name;
            using raw_type = std::decay_t<Type>;

            if constexpr (std::is_same_v<raw_type, int>)
            {
                serialize_int(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, float>)
            {
                serialize_float( value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, double>)
            {
                serialize_double(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, bool>)
            {
                serialize_bool(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, std::string>)
            {
                serialize_string(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, id_type>)
            {
                serialize_id_type(std::move(value));
                return true;
            }
            return false;
        }

        virtual void serialize_int(int serializable) override;
        virtual void serialize_float(float serializable) override;
        virtual void serialize_double(double serializable) override;
        virtual void serialize_bool(bool serializable) override;
        virtual void serialize_string(const std::string_view& serializable) override;
        virtual void serialize_id_type(id_type serializable) override;

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
        std::string val_name;

        yaml_view() = default;
        ~yaml_view() = default;

        //void start_object(std::string& name, id_type& typeID)
        //{

        //}
        //void end_object()
        //{

        //}

        template<typename Type>
        bool serialize(std::string& name, Type&& value)
        {
            val_name = name;
            using raw_type = std::decay_t<Type>;

            if constexpr (std::is_same_v<raw_type, int>)
            {
                serialize_int(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, float>)
            {
                serialize_float(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, double>)
            {
                serialize_double(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, bool>)
            {
                serialize_bool(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, std::string>)
            {
                serialize_string(value);
                return true;
            }
            else if constexpr (std::is_same_v<raw_type, id_type>)
            {
                serialize_id_type(std::move(value));
                return true;
            }
            return false;
        }

        virtual void serialize_int(int serializable) override;
        virtual void serialize_float(float serializable) override;
        virtual void serialize_double(double serializable) override;
        virtual void serialize_bool(bool serializable) override;
        virtual void serialize_string(const std::string_view& serializable) override;
        virtual void serialize_id_type(id_type serializable) override;

        virtual common::result<int, fs_error> deserialize_int(std::string& name) override;
        virtual common::result<float, fs_error> deserialize_float(std::string& name) override;
        virtual common::result<double, fs_error> deserialize_double(std::string& name) override;
        virtual bool deserialize_bool(std::string& name) override;
        virtual common::result<std::string, fs_error> deserialize_string(std::string& name) override;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string& name) override;
    };

}
#include <core/serialization/serializer_view.inl>

