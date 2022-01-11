#include <core/serialization/serialization.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename Type, typename ViewType>
    inline common::result<void, fs_error> serialize(ViewType& s_view, Type data)
    {
        return common::success;
    }

    template<typename ViewType, typename Type>
    inline common::result<void, fs_error> write(fs::view file, Type data)
    {
        return common::success;
    }

    template<typename ViewType, typename Type>
    inline common::result<void, fs_error> write(Type data, fs::view file)
    {
        return common::success;
    }


    template<typename Type, typename ViewType>
    common::result<void, fs_error> deserialize(ViewType& s_view)
    {
        return common::success;
    }

    template<typename ViewType, typename Type>
    inline common::result<void, fs_error> load(fs::view file)
    {
        return common::success;
    }

    template<typename ViewType = serializer_view&, typename Type>
    inline common::result<void, fs_error> load(std::vector<byte> data)
    {
        return common::success;
    }
    template<typename ViewType = serializer_view&, typename Iterator, typename Type>
    common::result<void, fs_error> load(Iterator begin, Iterator end)
    {
        return common::success;
    }
}
