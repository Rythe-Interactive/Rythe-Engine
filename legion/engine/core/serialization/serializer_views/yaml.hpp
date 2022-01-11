#pragma once
#include <core/serialization/serializer_views/serializer_view.hpp>

namespace legion::core::serialization
{

    struct yaml : public serializer_view
    {
        yaml() = default;
        virtual ~yaml() = default;

        virtual void serialize_int(const std::string& name, int serializable) override;
        virtual void serialize_float(const std::string& name, float serializable) override;
        virtual void serialize_double(const std::string& name, double serializable) override;
        virtual void serialize_bool(const std::string& name, bool serializable) override;
        virtual void serialize_string(const std::string& name, const std::string& serializable) override;
        virtual void serialize_id_type(const std::string& name, id_type serializable) override;

        virtual common::result<int, fs_error> deserialize_int(const std::string& name) override;
        virtual common::result<float, fs_error> deserialize_float(const std::string& name) override;
        virtual common::result<double, fs_error> deserialize_double(const std::string& name) override;
        virtual common::result<bool, fs_error> deserialize_bool(const std::string& name) override;
        virtual common::result<std::string, fs_error> deserialize_string(const std::string& name) override;
        virtual common::result<id_type, fs_error> deserialize_id_type(const std::string& name) override;

        virtual void start_object() override;
        virtual void start_object(const std::string& name) override;
        virtual void end_object() override;

        virtual void start_container(const std::string& name) override;
        virtual void end_container() override;

        virtual common::result<void, fs_error> start_read(const std::string& name) override;
        virtual void end_read() override;

        virtual bool is_current_array() override;
        virtual bool is_current_object() override;
        virtual size_type current_item_size() override;

        virtual std::string get_key() override;

        virtual common::result<void, fs_error> write(fs::view& file) override;
        virtual common::result<void, fs_error> read(const fs::view& file) override;
        virtual common::result<void, fs_error> read(const byte_vec& data) override;
        virtual common::result<void, fs_error> read(byte_vec::iterator begin, byte_vec::iterator end) override;
    };
}
