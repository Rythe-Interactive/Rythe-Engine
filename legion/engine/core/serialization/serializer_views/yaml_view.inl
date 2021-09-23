#include <core/serialization/serializer_views/yaml_view.hpp>
#pragma once

namespace legion::core::serialization
{
    void yaml_view::serialize_int(std::string& name, int serializable)
    {

    }

    void yaml_view::serialize_float(std::string& name, float serializable)
    {

    }

    void yaml_view::serialize_double(std::string& name, double serializable)
    {

    }

    void yaml_view::serialize_bool(std::string& name, bool serializable)
    {

    }

    void yaml_view::serialize_string(std::string& name, const std::string_view& serializable)
    {

    }

    void yaml_view::serialize_id_type(std::string& name, id_type serializable)
    {

    }

    common::result<int, fs_error> yaml_view::deserialize_int(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }
    common::result<float, fs_error> yaml_view::deserialize_float(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }
    common::result<double, fs_error> yaml_view::deserialize_double(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }
    common::result<void, fs_error>  yaml_view::deserialize_bool(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    common::result<std::string, fs_error> yaml_view::deserialize_string(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    common::result<id_type, fs_error> yaml_view::deserialize_id_type(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline void yaml_view::start_object()
    {
    }

    inline void yaml_view::start_object(std::string name)
    {
    }

    inline void yaml_view::end_object()
    {
    }

    inline void yaml_view::start_container(std::string name)
    {
    }

    inline void yaml_view::end_container()
    {
    }

    inline common::result<void, fs_error> yaml_view::write(const fs::view& file)
    {
        return legion_fs_error("Not yet implemented");
    }
    inline common::result<void, fs_error> yaml_view::load(const fs::view& file)
    {
        return legion_fs_error("Not yet implemented");
    }

}
