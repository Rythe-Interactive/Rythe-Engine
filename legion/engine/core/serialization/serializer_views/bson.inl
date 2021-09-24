#include <core/serialization/serializer_views/bson.hpp>
#pragma once

namespace legion::core::serialization
{
    inline void bson::serialize_int(std::string& name, int serializable)
    {

    }

    inline void bson::serialize_float(std::string& name, float serializable)
    {

    }

    inline void bson::serialize_double(std::string& name, double serializable)
    {

    }

    inline void bson::serialize_bool(std::string& name, bool serializable)
    {

    }

    inline void bson::serialize_string(std::string& name, const std::string_view& serializable)
    {

    }

    inline void bson::serialize_id_type(std::string& name, id_type serializable)
    {

    }

    inline common::result<int, fs_error> bson::deserialize_int(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<float, fs_error> bson::deserialize_float(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<double, fs_error> bson::deserialize_double(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<void, fs_error>  bson::deserialize_bool(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<std::string, fs_error> bson::deserialize_string(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline common::result<id_type, fs_error> bson::deserialize_id_type(std::string_view& name)
    {
        return legion_fs_error("not implemented");
    }

    inline void bson::start_object()
    {

    }

    inline void bson::start_object(std::string name)
    {

    }

    inline void bson::end_object()
    {

    }

    inline void bson::start_container(std::string name)
    {
    }

    inline void bson::end_container()
    {
    }

    inline common::result<void, fs_error> bson::write(fs::view& file)
    {
        return legion_fs_error("not yet implemented");
    }

    inline common::result<void, fs_error> bson::read(fs::view& file)
    {
        return legion_fs_error("not yet implemented");
    }

    inline common::result<void, fs_error> bson::read(byte_vec data)
    {
        return legion_fs_error("not yet implemented");
    }

    inline common::result<void, fs_error> bson::read(byte_vec::iterator begin, byte_vec::iterator end)
    {
        return legion_fs_error("not yet implemented");
    }
}

