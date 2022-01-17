#pragma once
#include <core/serialization/serializer_views/serializer_view.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <stack>

namespace legion::core::serialization
{

    struct yaml : public serializer_view
    {
    protected:
        struct entry
        {
            std::string key;
            YAML::Node item;
            size_type currentReadIndex = 0;

            RULE_OF_5_NOEXCEPT(entry);

            entry(const std::string& k, const YAML::Node& j, size_type idx = 0) : key(k), item(j), currentReadIndex(idx) {}
        };

        entry root;
        bool isPeaking;
        entry peakObject;
        std::stack<entry> active_stack;

        entry& current_item();

    public:
        yaml() = default;
        virtual ~yaml() = default;

        virtual void serialize_int(const std::string& name, int serializable) override;
        virtual void serialize_float(const std::string& name, float serializable) override;
        virtual void serialize_double(const std::string& name, double serializable) override;
        virtual void serialize_bool(const std::string& name, bool serializable) override;
        virtual void serialize_string(const std::string& name, const std::string& serializable) override;
        virtual void serialize_id_type(const std::string& name, id_type serializable) override;

        L_NODISCARD virtual common::result<int, fs_error> deserialize_int(const std::string& name) override;
        L_NODISCARD virtual common::result<float, fs_error> deserialize_float(const std::string& name) override;
        L_NODISCARD virtual common::result<double, fs_error> deserialize_double(const std::string& name) override;
        L_NODISCARD virtual common::result<bool, fs_error> deserialize_bool(const std::string& name) override;
        L_NODISCARD virtual common::result<std::string, fs_error> deserialize_string(const std::string& name) override;
        L_NODISCARD virtual common::result<id_type, fs_error> deserialize_id_type(const std::string& name) override;

        virtual void start_object() override;
        virtual void start_object(const std::string& name) override;
        virtual void end_object() override;

        virtual void start_container(const std::string& name) override;
        virtual void end_container() override;

        L_NODISCARD virtual common::result<void, fs_error> start_read(const std::string& name) override;
        L_NODISCARD virtual common::result<void, fs_error> peak_ahead(const std::string& name) override;
        virtual void end_read() override;

        L_NODISCARD virtual bool is_current_array() override;
        L_NODISCARD virtual bool is_current_object() override;
        L_NODISCARD virtual size_type current_item_size() override;

        L_NODISCARD virtual std::string get_key() override;

        L_NODISCARD virtual common::result<void, fs_error> write(fs::view& file) override;
        L_NODISCARD virtual common::result<void, fs_error> read(const fs::view& file) override;
        L_NODISCARD virtual common::result<void, fs_error> read(const byte_vec& data) override;
        L_NODISCARD virtual common::result<void, fs_error> read(byte_vec::iterator begin, byte_vec::iterator end) override;
    };
}
