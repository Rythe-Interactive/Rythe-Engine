#pragma once
#include <core/types/meta.hpp>
#include <core/filesystem/filesystem.hpp>
#include <core/serialization/serializer_view.hpp>
#include <core/serialization/prototype.hpp>

#include <nlohmann/json.hpp>

#include <fstream>
#include <any>

namespace legion::core::serialization
{
    using json = nlohmann::ordered_json;

    struct serializer_base
    {
        serializer_base() = default;
        ~serializer_base() = default;

        virtual bool serialize(const std::any& serializable, serializer_view& view, std::string name) = 0;
        virtual prototype_base deserialize(serializer_view& view) = 0;
    };

    template<typename serializable_type>
    struct serializer : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual bool serialize(const std::any& serializable, serializer_view& view, std::string name) override;
        virtual prototype_base deserialize(serializer_view& view) override;

        bool serialize_container(const std::any& container, serializer_view& view);

        bool write(const std::any& serializable, std::string name, const fs::view& file);
        bool read(const fs::view& view);
    };

    template<>
    struct serializer<ecs::entity_data> : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual bool serialize(const std::any& serializable, serializer_view& view, std::string name) override;
        virtual prototype_base deserialize(serializer_view& view) override;

        bool serialize_ent_data(const ecs::entity_data& serializable, serializer_view& view, std::string name);
    };

    template<>
    struct serializer<ecs::entity> : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual bool serialize(const std::any& serializable, serializer_view& view, std::string name) override;
        virtual prototype_base deserialize(serializer_view& view) override;

        bool serialize_ent(const ecs::entity& serializable, serializer_view& view, std::string name);
    };

}

//#include <core/serialization/serializer.inl>




