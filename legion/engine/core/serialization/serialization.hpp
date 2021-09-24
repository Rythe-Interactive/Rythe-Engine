#pragma once
#include <core/common/result.hpp>
#include <core/serialization/serializationregistry.hpp>

#include <string>
#include <string_view>

namespace legion::core::serialization
{
    template<typename Type, typename ViewType>
    common::result<void, fs_error> serialize(ViewType& s_view, Type data);

    template<typename ViewType = serializer_view&, typename Type>
    common::result<void, fs_error> write(fs::view file, Type data);

    template<typename ViewType = serializer_view&, typename Type>
    common::result<void, fs_error> write(Type data, fs::view file);


    template<typename Type, typename ViewType>
    common::result<void, fs_error> deserialize(ViewType& s_view);

    template<typename ViewType = serializer_view&, typename Type>
    common::result<void, fs_error> load(fs::view file);

    template<typename ViewType = serializer_view&, typename Type>
    common::result<void, fs_error> load(std::vector<byte> data);

    template<typename ViewType = serializer_view&, typename Type, typename Iterator>
    common::result<void, fs_error> load(Iterator begin, Iterator end);
}

#include <core/serialization/serializer.inl>
