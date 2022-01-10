#pragma once
#include <core/ecs/ecs.hpp>
#include<core/types/meta.hpp>
#include <core/filesystem/filesystem.hpp>
#include <core/serialization/serializer_views/json.hpp>
#include <core/serialization/serializer_views/bson.hpp>
#include <core/serialization/serializer_views/yaml.hpp>

#include <fstream>

namespace legion::core::serialization
{
    struct serializer_base
    {
        serializer_base() = default;
        ~serializer_base() = default;

        virtual common::result<void, fs_error> serialize(const void* serializable, serializer_view& view, const std::string& name) LEGION_PURE;
        virtual common::result<void, fs_error> deserialize(void* target, serializer_view& view, const std::string& name) LEGION_PURE;
        virtual id_type type_size() LEGION_PURE;
    };

    template<typename serializable_type>
    struct serializer : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual common::result<void, fs_error> serialize(const void* serializable, serializer_view& s_view, const std::string& name) override;
        virtual common::result<void, fs_error> deserialize(void* target, serializer_view& s_view, const std::string& name) override;
        virtual id_type type_size() override { return sizeof(serializable_type); }
    };

    template<>
    struct serializer<ecs::entity_data> : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual common::result<void, fs_error> serialize(const void* serializable, serializer_view& view, const std::string& name) override;
        virtual common::result<void, fs_error> deserialize(void* target, serializer_view& s_view, const std::string& name) override;
        virtual id_type type_size() override { return sizeof(ecs::entity_data); }
    };

    template<>
    struct serializer<ecs::entity> : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual common::result<void, fs_error> serialize(const void* serializable, serializer_view& view, const std::string& name) override;
        virtual common::result<void, fs_error> deserialize(void* target, serializer_view& s_view, const std::string& name) override;
        virtual id_type type_size() override { return sizeof(ecs::entity); }
    };

}





