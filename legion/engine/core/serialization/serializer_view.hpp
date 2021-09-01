#pragma once
#include <core/core.hpp>
#include <nlohmann/json.hpp>

namespace legion::core::serialization
{
    using json = nlohmann::ordered_json;

    struct serializer_view
    {
        serializer_view() = default;
        virtual ~serializer_view() = default;

        template<typename Type>
        bool serialize(Type&& value)
        {
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
                serialize_id_type(value);
                return true;
            }
            return false;
        }

        void start_object(id_type typeId);
        void end_object();

        virtual void serialize_int(int serializable) = 0;
        virtual void serialize_float(float serializable) = 0;
        virtual void serialize_double(double serializable) = 0;
        virtual void serialize_bool(bool serializable) = 0;
        virtual void serialize_string(const std::string_view& serializable) = 0;
        virtual void serialize_id_type(id_type serializable) = 0;

        virtual void write_result(fs::view& file) = 0;

        virtual void load_file(fs::view& file) = 0;

        virtual id_type deserialize_object_id() = 0;
        virtual int deserialize_int() = 0; // these should return a result type with errors
        virtual float deserialize_float() = 0;
        virtual double deserialize_double() = 0;
        virtual bool deserialize_bool() = 0;
        virtual std::string deserialize_string() = 0;
        virtual id_type deserialize_id_type() = 0;
    };

    struct json_view : serializer_view
    {
        json data;
        virtual void serialize_int(int serializable) override
        {
            data << serializable;
        }
        virtual void serialize_float(float serializable) override
        {
            data << serializable;
        }
        virtual void serialize_double(double serializable) override
        {
            data << serializable;
        }
        virtual void serialize_bool(bool serializable) override
        {
            data << serializable;
        }
        virtual void serialize_string(const std::string_view& serializable) override
        {
            data << serializable;
        }
        virtual void serialize_id_type(id_type serializable) override
        {
            data << serializable;
        }

        virtual int deserialize_int() override
        {
        }
        virtual float deserialize_float() override
        {
        }
        virtual double deserialize_double() override
        {
        }
        virtual bool deserialize_bool() override
        {
        }
        virtual std::string deserialize_string() override
        {
        }
        virtual id_type deserialize_id_type() override
        {
        }
    };

    struct bson_view : serializer_view
    {
        virtual void serialize_int(int serializable) override
        {
        }
        virtual void serialize_float(float serializable) override
        {
        }
        virtual void serialize_double(double serializable) override
        {
        }
        virtual void serialize_bool(bool serializable) override
        {
        }
        virtual void serialize_string(const std::string_view& serializable) override
        {
        }
        virtual void serialize_id_type(id_type serializable) override
        {
        }

        virtual int deserialize_int() override
        {
        }
        virtual float deserialize_float() override
        {
        }
        virtual double deserialize_double() override
        {
        }
        virtual bool deserialize_bool() override
        {
        }
        virtual std::string deserialize_string() override
        {
        }
        virtual id_type deserialize_id_type() override
        {
        }
    };

    struct yaml_view : serializer_view
    {
        virtual void serialize_int(int serializable) override
        {
        }
        virtual void serialize_float(float serializable) override
        {
        }
        virtual void serialize_double(double serializable) override
        {
        }
        virtual void serialize_bool(bool serializable) override
        {
        }
        virtual void serialize_string(const std::string_view& serializable) override
        {
        }
        virtual void serialize_id_type(id_type serializable) override
        {
        }

        virtual int deserialize_int() override
        {
        }
        virtual float deserialize_float() override
        {
        }
        virtual double deserialize_double() override
        {
        }
        virtual bool deserialize_bool() override
        {
        }
        virtual std::string deserialize_string() override
        {
        }
        virtual id_type deserialize_id_type() override
        {
        }
    };
}

