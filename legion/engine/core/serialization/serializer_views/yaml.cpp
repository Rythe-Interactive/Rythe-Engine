#include <core/serialization/serializer_views/yaml.hpp>

namespace legion::core::serialization
{
    void yaml::serialize_int(const std::string& name, int serializable)
    {
    }

    void yaml::serialize_float(const std::string& name, float serializable)
    {
    }

    void yaml::serialize_double(const std::string& name, double serializable)
    {
    }

    void yaml::serialize_bool(const std::string& name, bool serializable)
    {
    }

    void yaml::serialize_string(const std::string& name, const std::string& serializable)
    {
    }

    void yaml::serialize_id_type(const std::string& name, id_type serializable)
    {
    }

    common::result<int, fs_error> yaml::deserialize_int(const std::string& name)
    {
        return legion_fs_error("not yet implemented");
    }

    common::result<float, fs_error> yaml::deserialize_float(const std::string& name)
    {
        return legion_fs_error("not yet implemented");
    }

    common::result<double, fs_error> yaml::deserialize_double(const std::string& name)
    {
        return legion_fs_error("not yet implemented");
    }

    common::result<bool, fs_error> yaml::deserialize_bool(const std::string& name)
    {
        return legion_fs_error("not yet implemented");
    }

    common::result<std::string, fs_error> yaml::deserialize_string(const std::string& name)
    {
        return legion_fs_error("not yet implemented");
    }

    common::result<id_type, fs_error> yaml::deserialize_id_type(const std::string& name)
    {
        return legion_fs_error("not yet implemented");
    }

    void yaml::start_object()
    {
    }

    void yaml::start_object(const std::string& name)
    {
    }

    void yaml::end_object()
    {
    }

    void yaml::start_container(const std::string& name)
    {
    }

    void yaml::end_container()
    {
    }

    common::result<void, fs_error> yaml::start_read(const std::string& name)
    {
        return legion_fs_error("not yet implemented");
    }

    void yaml::end_read()
    {
    }

    bool yaml::is_current_array()
    {
        return false;
    }

    bool yaml::is_current_object()
    {
        return false;
    }

    size_type yaml::current_item_size()
    {
        return size_type();
    }

    std::string yaml::get_key()
    {
        return std::string();
    }

    common::result<void, fs_error> yaml::write(fs::view& file)
    {
        return legion_fs_error("not yet implemented");
    }

    common::result<void, fs_error> yaml::read(const fs::view& file)
    {
        return legion_fs_error("not yet implemented");
    }

    common::result<void, fs_error> yaml::read(const byte_vec& data)
    {
        return legion_fs_error("not yet implemented");
    }

    common::result<void, fs_error> yaml::read(byte_vec::iterator begin, byte_vec::iterator end)
    {
        return legion_fs_error("not yet implemented");
    }

}
