#include <core/serialization/serializer_views/bson_view.hpp>
#pragma once

namespace legion::core::serialization
{
    inline void bson_view::serialize_int(std::string& name, int serializable)
    {

    }

    inline void bson_view::serialize_float(std::string& name, float serializable)
    {

    }

    inline void bson_view::serialize_double(std::string& name, double serializable)
    {

    }

    inline void bson_view::serialize_bool(std::string& name, bool serializable)
    {

    }

    inline void bson_view::serialize_string(std::string& name, const std::string_view& serializable)
    {

    }

    inline void bson_view::serialize_id_type(std::string& name, id_type serializable)
    {

    }

    inline common::result<int, fs_error> bson_view::deserialize_int(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<float, fs_error> bson_view::deserialize_float(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<double, fs_error> bson_view::deserialize_double(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<void, fs_error>  bson_view::deserialize_bool(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<std::string, fs_error> bson_view::deserialize_string(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<id_type, fs_error> bson_view::deserialize_id_type(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline void bson_view::start_object()
    {

    }

    inline void bson_view::start_object(std::string name)
    {

    }

    inline void bson_view::end_object()
    {

    }

    inline void bson_view::start_container(std::string name)
    {
    }

    inline void bson_view::end_container()
    {
    }

    inline common::result<void, fs_error> bson_view::write(const fs::view& file)
    {
        return legion_fs_error("not yet implemented");
    }

    inline common::result<void, fs_error> bson_view::load(const fs::view& file)
    {
        return legion_fs_error("not yet implemented");
    }
}

