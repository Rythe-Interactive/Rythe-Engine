#pragma once
#include <core/ecs/ecs.hpp>
#include<core/types/meta.hpp>
#include <core/filesystem/filesystem.hpp>
#include <core/serialization/serializer_views/json_view.hpp>
#include <core/serialization/serializer_views/bson_view.hpp>
#include <core/serialization/serializer_views/yaml_view.hpp>
#include <core/serialization/prototype.hpp>

#include <fstream>

namespace legion::core::serialization
{
    using json = nlohmann::ordered_json;

    struct serializer_base
    {
        serializer_base() = default;
        ~serializer_base() = default;

        virtual common::result<void, fs_error>  serialize(const void* serializable, serializer_view& view, std::string name) = 0;
        virtual prototype_base deserialize(serializer_view& view) = 0;
    };

    template<typename serializable_type>
    struct serializer : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual common::result<void, fs_error> serialize(const void* serializable, serializer_view& s_view, std::string name) override;
        virtual prototype_base deserialize(serializer_view& s_view) override;
    };

    template<>
    struct serializer<ecs::entity_data> : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual common::result<void, fs_error> serialize(const void* serializable, serializer_view& view, std::string name) override;
        virtual prototype_base deserialize(serializer_view& view) override;
    };

    template<>
    struct serializer<ecs::entity> : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual common::result<void, fs_error> serialize(const void* serializable, serializer_view& view, std::string name) override;
        virtual prototype_base deserialize(serializer_view& view) override;
    };

}





