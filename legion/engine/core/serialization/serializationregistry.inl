#include <core/serialization/serializationregistry.hpp>
#pragma once

namespace legion::core::serialization
{
    std::unordered_map<id_type, std::unique_ptr<serializer_base>> serializer_registry::serializers;

    template<typename type>
    inline pointer<serializer<type>> serializer_registry::register_serializer()
    {
        serializer_registry::serializers.emplace(type_hash<type>().local(), std::make_unique<serializer<type>>());
        id_type typeId = type_hash<type>().local();
        if (serializers.count(typeId))
            return { dynamic_cast<serializer<type>*>(serializers.at(typeId).get()) };
        return { nullptr };
    }

    template<typename type>
    inline pointer<serializer<type>> serializer_registry::get_serializer()
    {
        id_type typeId = type_hash<type>().local();
        if (serializers.count(typeId))
            return { dynamic_cast<serializer<type>*>(serializers.at(typeId).get()) };
        return register_serializer<type>();
    }

    inline pointer<serializer_base> serializer_registry::get_serializer(id_type typeId)
    {
        if (serializers.count(typeId))
            return { serializers.at(typeId).get() };
        return { nullptr };
    }

    template<typename T>
    common::result<void, fs_error> serializer_registry::write(T data, serializer_view& s_view)
    {
        auto serializer = get_serializer<T>();
        serializer->serialize(&data,s_view, "");
        return s_view.write();
    }

    template<typename T>
    common::result<void, fs_error> serializer_registry::write(T data, std::string_view& filePath)
    {
        fs::view file(filePath);

        auto result = file.get_extension();

        if (result.has_error())
            return legion_fs_error(result.error().what());

        if (result.value() == ".json")
        {
            auto s_view = json_view(filePath);
            return write(data, s_view);
        } 
        else if (result.value() == ".bson")
        {
            auto s_view = bson_view(filePath);
            return write(data, s_view);
        }
        else if (result.value() == ".yaml")
        {
            auto s_view = yaml_view(filePath);
            return write(data, s_view);
        }
    }

    template<typename T>
    common::result<void, fs_error> serializer_registry::write(T data)
    {
        std::string folder = "assets://serialization_output/output.json";
        std::string_view filePath = folder;
        return write(data, filePath);
    }
}
