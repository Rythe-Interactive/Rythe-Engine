#pragma once
#include <core/common/common.hpp>
#include <core/types/types.hpp>
#include <core/filesystem/filesystem.hpp>

#include <iostream>

namespace legion::core::serialization
{
    struct serializer_view
    {
        serializer_view() = default;;
        virtual ~serializer_view() = default;

        template<typename Type>
        bool serialize(std::string name, Type&& value);

        virtual void serialize_int(std::string& name, int serializable) LEGION_PURE;
        virtual void serialize_float(std::string& name, float serializable) LEGION_PURE;
        virtual void serialize_double(std::string& name, double serializable) LEGION_PURE;
        virtual void serialize_bool(std::string& name, bool serializable) LEGION_PURE;
        virtual void serialize_string(std::string& name, const std::string_view& serializable) LEGION_PURE;
        virtual void serialize_id_type(std::string& name, id_type serializable) LEGION_PURE;

        template<typename Type>
        common::result<Type> deserialize(std::string_view& name);

        virtual common::result<int, fs_error> deserialize_int(std::string_view& name) LEGION_PURE;
        virtual common::result<float, fs_error> deserialize_float(std::string_view& name) LEGION_PURE;
        virtual common::result<double, fs_error> deserialize_double(std::string_view& name) LEGION_PURE;
        virtual common::result<void, fs_error>  deserialize_bool(std::string_view& name) LEGION_PURE;
        virtual common::result<std::string, fs_error> deserialize_string(std::string_view& name) LEGION_PURE;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string_view& name) LEGION_PURE;

        virtual void start_object() LEGION_PURE;
        virtual void start_object(std::string name) LEGION_PURE;
        virtual void end_object() LEGION_PURE;

        virtual void start_container(std::string name) LEGION_PURE;
        virtual void end_container() LEGION_PURE;

        virtual common::result<void, fs_error> write(fs::view& file) LEGION_PURE;
        virtual common::result<void, fs_error> load(fs::view& file) LEGION_PURE;
    };
}

#include <core/serialization/serializer_views/serializer_view.inl>

