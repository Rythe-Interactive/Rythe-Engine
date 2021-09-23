#pragma once
#include <core/serialization/serializer_views/serializer_view.hpp>

namespace legion::core::serialization
{

    struct yaml_view : public serializer_view
    {
        yaml_view() = default;
        virtual ~yaml_view() = default;

        virtual void serialize_int(std::string& name, int serializable) override;
        virtual void serialize_float(std::string& name, float serializable) override;
        virtual void serialize_double(std::string& name, double serializable) override;
        virtual void serialize_bool(std::string& name, bool serializable) override;
        virtual void serialize_string(std::string& name, const std::string_view& serializable) override;
        virtual void serialize_id_type(std::string& name, id_type serializable) override;

        virtual common::result<int, fs_error> deserialize_int(std::string_view& name) override;
        virtual common::result<float, fs_error> deserialize_float(std::string_view& name) override;
        virtual common::result<double, fs_error> deserialize_double(std::string_view& name) override;
        virtual common::result<void, fs_error>  deserialize_bool(std::string_view& name) override;
        virtual common::result<std::string, fs_error> deserialize_string(std::string_view& name) override;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string_view& name) override;

        virtual void start_object() override;
        virtual void start_object(std::string name) override;
        virtual void end_object() override;

        virtual void start_container(std::string name) override;
        virtual void end_container() override;

        virtual common::result<void, fs_error> write(fs::view& file) override;
        virtual common::result<void, fs_error> load(fs::view& file) override;
    };
}
#include <core/serialization/serializer_views/yaml_view.inl>

