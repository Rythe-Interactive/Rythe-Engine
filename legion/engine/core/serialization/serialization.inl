#include <core/serialization/serialization.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename ViewType, typename Type>
    inline common::result<void, fs_error> serialize(ViewType& s_view, Type&& data, std::string_view name)
    {
        std::vector<std::string> warnings{};

        auto serializer = SerializerRegistry::getSerializer<remove_cvr_t<Type>>();
        auto result = serializer->serialize(&data, s_view, name);
        PropagateErrors(result, warnings);

        return { common::success, warnings };
    }

    template<typename ViewType, typename Type>
    inline common::result<void, fs_error> write(fs::view file, Type&& data, std::string_view name)
    {
        ViewType view{};
        std::vector<std::string> warnings{};

        {
            auto serializer = SerializerRegistry::getSerializer<remove_cvr_t<Type>>();
            auto result = serializer->serialize(&data, view, name);
            PropagateErrors(result, warnings);
        }

        {
            auto result = view.write(file);
            PropagateErrors(result, warnings);
        }

        return { common::success, warnings };
    }

    template<typename ViewType, typename Type>
    inline L_ALWAYS_INLINE common::result<void, fs_error> write(Type&& data, fs::view file, std::string_view name)
    {
        return write(file, std::forward<Type>(data), name);
    }

    template<typename Type, typename ViewType>
    common::result<Type, fs_error> deserialize(ViewType& s_view, std::string_view name)
    {
        byte rawData[sizeof(Type)];
        Type* data = reinterpret_cast<Type*>(rawData);
        std::vector<std::string> warnings{};

        auto serializer = SerializerRegistry::getSerializer<remove_cvr_t<Type>>();
        auto result = serializer->deserialize(data, s_view, name);
        PropagateErrors(result, warnings);

        return { *data, warnings };
    }

    template<typename ViewType, typename Type>
    inline common::result<Type, fs_error> load(const fs::view& file, std::string_view name)
    {
        static_assert(!std::is_pointer_v<Type> && !std::is_reference_v<Type>, "Type to load must be a value type.");
        static_assert(!std::is_abstract_v<Type>, "Type to load must be a constructible type.");

        ViewType view{};
        byte rawData[sizeof(Type)];
        Type* data = reinterpret_cast<Type*>(rawData);
        std::vector<std::string> warnings{};

        {
            auto result = view.read(file);
            PropagateErrors(result, warnings);
        }

        {
            auto serializer = SerializerRegistry::getSerializer<remove_cvr_t<Type>>();
            auto result = serializer->deserialize(data, view, name);
            PropagateErrors(result, warnings);
        }

        return { *data, warnings };
    }

    template<typename ViewType, typename Type>
    inline common::result<Type, fs_error> load(const byte_vec& bytes, std::string_view name)
    {
        static_assert(!std::is_pointer_v<Type> && !std::is_reference_v<Type>, "Type to load must be a value type.");
        static_assert(!std::is_abstract_v<Type>, "Type to load must be a constructible type.");

        ViewType view{};
        byte rawData[sizeof(Type)];
        Type* data = reinterpret_cast<Type*>(rawData);
        std::vector<std::string> warnings{};

        {
            auto result = view.read(bytes);
            PropagateErrors(result, warnings);
        }

        {
            auto serializer = SerializerRegistry::getSerializer<remove_cvr_t<Type>>();
            auto result = serializer->deserialize(data, view, name);
            PropagateErrors(result, warnings);
        }

        return { *data, warnings };
    }

    template<typename ViewType, typename Iterator, typename Type>
    common::result<Type, fs_error> load(Iterator begin, Iterator end, std::string_view name)
    {
        static_assert(!std::is_pointer_v<Type> && !std::is_reference_v<Type>, "Type to load must be a value type.");
        static_assert(!std::is_abstract_v<Type>, "Type to load must be a constructible type.");

        ViewType view{};
        byte rawData[sizeof(Type)];
        Type* data = reinterpret_cast<Type*>(rawData);
        std::vector<std::string> warnings{};

        {
            auto result = view.read(begin, end);
            PropagateErrors(result, warnings);
        }

        {
            auto serializer = SerializerRegistry::getSerializer<remove_cvr_t<Type>>();
            auto result = serializer->deserialize(data, view, name);
            PropagateErrors(result, warnings);
        }

        return { *data, warnings };
    }
}
