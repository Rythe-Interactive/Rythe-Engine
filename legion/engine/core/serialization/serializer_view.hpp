#pragma once
#include <core/core.hpp>

#include <stack>
#include <nlohmann/json.hpp>

#include <iostream>

namespace legion::core::serialization
{
    using json = nlohmann::ordered_json;

    struct serializer_view
    {
        fs::view file;
        serializer_view(std::string_view filePath) : file(filePath){};
        virtual ~serializer_view() = default;

        template<typename Type>
        bool serialize(std::string name, Type&& value);

        virtual void serialize_int(std::string& name, int serializable) LEGION_PURE;
        virtual void serialize_float(std::string& name, float serializable) = 0;
        virtual void serialize_double(std::string& name, double serializable) = 0;
        virtual void serialize_bool(std::string& name, bool serializable) = 0;
        virtual void serialize_string(std::string& name, const std::string_view& serializable) = 0;
        virtual void serialize_id_type(std::string& name, id_type serializable) = 0;

        template<typename Type>
        common::result<Type> deserialize(std::string_view& name);

        virtual common::result<int, fs_error> deserialize_int(std::string_view& name) = 0;
        virtual common::result<float, fs_error> deserialize_float(std::string_view& name) = 0;
        virtual common::result<double, fs_error> deserialize_double(std::string_view& name) = 0;
        virtual bool deserialize_bool(std::string_view& name) = 0;
        virtual common::result<std::string, fs_error> deserialize_string(std::string_view& name) = 0;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string_view& name) = 0;

        virtual void start_object() = 0;
        virtual void start_object(std::string name) = 0;
        virtual void end_object() = 0;

        virtual void start_container(std::string name) = 0;
        virtual void end_container() = 0;

        virtual common::result<void, fs_error> write() = 0;
        virtual common::result<void, fs_error> load(fs::view& file) = 0;
    };

    struct json_view : public serializer_view
    {
        json root;
        std::stack<json> write_queue;

        json_view(std::string_view filePath) : serializer_view(filePath) { };
        ~json_view() = default;

        virtual void serialize_int(std::string& name, int serializable) override;
        virtual void serialize_float(std::string& name, float serializable) override;
        virtual void serialize_double(std::string& name, double serializable) override;
        virtual void serialize_bool(std::string& name, bool serializable) override;
        virtual void serialize_string(std::string& name, const std::string_view& serializable) override;
        virtual void serialize_id_type(std::string& name, id_type serializable) override;

        virtual common::result<int, fs_error> deserialize_int(std::string_view& name) override;
        virtual common::result<float, fs_error> deserialize_float(std::string_view& name) override;
        virtual common::result<double, fs_error> deserialize_double(std::string_view& name) override;
        virtual bool deserialize_bool(std::string_view& name) override;
        virtual common::result<std::string, fs_error> deserialize_string(std::string_view& name) override;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string_view& name) override;

        virtual void start_object() override;
        virtual void start_object(std::string name) override;
        virtual void end_object() override;

        virtual void start_container(std::string name) override;
        virtual void end_container() override;

        virtual common::result<void, fs_error> write() override;
        virtual common::result<void, fs_error> load(fs::view& file) override;
    };

    struct bson_view : public serializer_view
    {

        bson_view(std::string_view filePath) : serializer_view(filePath) {};
        ~bson_view() = default;

        virtual void serialize_int(std::string& name, int serializable) override;
        virtual void serialize_float(std::string& name, float serializable) override;
        virtual void serialize_double(std::string& name, double serializable) override;
        virtual void serialize_bool(std::string& name, bool serializable) override;
        virtual void serialize_string(std::string& name, const std::string_view& serializable) override;
        virtual void serialize_id_type(std::string& name, id_type serializable) override;

        virtual common::result<int, fs_error> deserialize_int(std::string_view& name) override;
        virtual common::result<float, fs_error> deserialize_float(std::string_view& name) override;
        virtual common::result<double, fs_error> deserialize_double(std::string_view& name) override;
        virtual bool deserialize_bool(std::string_view& name) override;
        virtual common::result<std::string, fs_error> deserialize_string(std::string_view& name) override;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string_view& name) override;

        virtual void start_object() override;
        virtual void start_object(std::string name) override;
        virtual void end_object() override;

        virtual void start_container(std::string name) override;
        virtual void end_container() override;

        virtual common::result<void, fs_error> write() override;
        virtual common::result<void, fs_error> load(fs::view& file) override;
    };

    struct yaml_view : public serializer_view
    {
        yaml_view(std::string_view filePath) : serializer_view(filePath) {};
        ~yaml_view() = default;

        virtual void serialize_int(std::string& name, int serializable) override;
        virtual void serialize_float(std::string& name, float serializable) override;
        virtual void serialize_double(std::string& name, double serializable) override;
        virtual void serialize_bool(std::string& name, bool serializable) override;
        virtual void serialize_string(std::string& name, const std::string_view& serializable) override;
        virtual void serialize_id_type(std::string& name, id_type serializable) override;

        virtual common::result<int, fs_error> deserialize_int(std::string_view& name) override;
        virtual common::result<float, fs_error> deserialize_float(std::string_view& name) override;
        virtual common::result<double, fs_error> deserialize_double(std::string_view& name) override;
        virtual bool deserialize_bool(std::string_view& name) override;
        virtual common::result<std::string, fs_error> deserialize_string(std::string_view& name) override;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string_view& name) override;

        virtual void start_object() override;
        virtual void start_object(std::string name) override;
        virtual void end_object() override;

        virtual void start_container(std::string name) override;
        virtual void end_container() override;

        virtual common::result<void, fs_error> write() override;
        virtual common::result<void, fs_error> load(fs::view& file) override;
    };

   
}

#include <core/serialization/serializer_view.inl>

