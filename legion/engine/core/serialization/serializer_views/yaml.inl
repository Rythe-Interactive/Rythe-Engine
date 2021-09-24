#include <core/serialization/serializer_views/yaml.hpp>
#pragma once

namespace legion::core::serialization
{
    inline void yaml::serialize_int(std::string& name, int serializable)
    {

    }

    inline void yaml::serialize_float(std::string& name, float serializable)
    {

    }

    inline void yaml::serialize_double(std::string& name, double serializable)
    {

    }

    inline void yaml::serialize_bool(std::string& name, bool serializable)
    {

    }

    inline void yaml::serialize_string(std::string& name, const std::string_view& serializable)
    {

    }

    inline void yaml::serialize_id_type(std::string& name, id_type serializable)
    {

    }

    inline common::result<int, fs_error> yaml::deserialize_int(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }
    inline common::result<float, fs_error> yaml::deserialize_float(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }
    inline common::result<double, fs_error> yaml::deserialize_double(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }
    inline common::result<void, fs_error>  yaml::deserialize_bool(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<std::string, fs_error> yaml::deserialize_string(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<id_type, fs_error> yaml::deserialize_id_type(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline void yaml::start_object()
    {
    }

    inline void yaml::start_object(std::string name)
    {

    }

    inline void yaml::end_object()
    {
    }

    inline void yaml::start_container(std::string name)
    {
    }

    inline void yaml::end_container()
    {
    }

    inline common::result<void, fs_error> yaml::write(fs::view& file)
    {
        return legion_fs_error("not yet implemented");
    }


    inline common::result<void, fs_error> yaml::read(fs::view& file)
    {
        return legion_fs_error("not yet implemented");
    }

    inline common::result<void, fs_error> yaml::read(byte_vec data)
    {
        return legion_fs_error("not yet implemented");
    }

    inline common::result<void, fs_error> yaml::read(byte_vec::iterator begin, byte_vec::iterator end)
    {
        return legion_fs_error("not yet implemented");
    }

}
