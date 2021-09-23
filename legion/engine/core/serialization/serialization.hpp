#pragma once
#include <core/common/result.hpp>
#include <core/serialization/serializationregistry.hpp>

#include <string>
#include <string_view>

namespace legion::core::serialization
{
    template<typename Type, typename view_type>
    common::result<void, fs_error> serialize(view_type& s_view, Type data)
    {
        auto serializer = serializer_registry::get_serializer<Type>();
        auto result = serializer->serialize(&data, s_view, "");
        return result;
    }

    template<typename view_type = serializer_view, typename Type>
    common::result<void, fs_error> write(std::string_view& filePath,Type data)
    {
        auto s_view = view_type();
        auto result = serialize(s_view, data);
        if (result.has_error())
            return legion_fs_error(result.error().what());

        fs::view file(filePath);
        return s_view.write(file);
    }

    template<typename view_type = serializer_view, typename Type>
    common::result<void, fs_error> write(Type data, std::string_view& filePath)
    {
        return write<view_type>(filePath,data);
    }


}
