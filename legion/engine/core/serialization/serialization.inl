#include <core/serialization/serialization.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename Type, typename ViewType>
    inline common::result<void, fs_error> serialize(ViewType& s_view, Type data)
    {
        auto serializer = serializer_registry::get_serializer<Type>();
        auto result = serializer->serialize(&data, s_view, "");
        return result;
    }

    template<typename ViewType = serializer_view&, typename Type>
    inline common::result<void, fs_error> write(fs::view file, Type data)
    {
        auto s_view = ViewType();
        auto result = serialize(s_view, data);
        if (result.has_error())
            return legion_fs_error(result.error().what());

        return s_view.write(file);
    }

    template<typename ViewType = serializer_view&, typename Type>
    inline common::result<void, fs_error> write(Type data, fs::view file)
    {
        return write<ViewType>(file, data);
    }


    template<typename Type, typename ViewType>
    common::result<void, fs_error> deserialize(ViewType& s_view)
    {
        return common::result<void, fs_error>();
    }

    template<typename ViewType, typename Type>
    inline common::result<void, fs_error> load(fs::view file)
    {
        return common::result<void, fs_error>();
    }

    template<typename ViewType = serializer_view&, typename Type>
    inline common::result<void, fs_error> load(std::vector<byte> data)
    {
        return common::result<void, fs_error>();
    }
    template<typename ViewType = serializer_view&, typename Iterator, typename Type>
    common::result<void, fs_error> load(Iterator begin, Iterator end)
    {
        return common::result<void, fs_error>();
    }
}
