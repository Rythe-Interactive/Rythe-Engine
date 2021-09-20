#pragma once
#include<core/types/meta.hpp>
#include <core/filesystem/filesystem.hpp>
#include <core/serialization/serializer_view.hpp>
#include <core/serialization/prototype.hpp>

#include <nlohmann/json.hpp>

#include <fstream>

namespace legion::core::serialization
{
    using json = nlohmann::ordered_json;

    struct serializer_base
    {
        serializer_base() = default;
        ~serializer_base() = default;

        virtual bool serialize(const void* serializable, serializer_view& view, std::string name) = 0;
        virtual prototype_base deserialize(serializer_view& view) = 0;
    };

    template<typename serializable_type>
    struct serializer : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual bool serialize(const void* serializable, serializer_view& s_view, std::string name) override;
        virtual prototype_base deserialize(serializer_view& s_view) override;

        bool serialize_container(const void* container, serializer_view& s_view, std::string& name);

        bool write(const void* serializable, std::string name, fs::view& file);
        bool read(const fs::view& view);
    };

    template<>
    struct serializer<ecs::entity_data> : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual bool serialize(const void* serializable, serializer_view& view, std::string name) override;
        virtual prototype_base deserialize(serializer_view& view) override;
    };

    template<>
    struct serializer<ecs::entity> : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual bool serialize(const void* serializable, serializer_view& view, std::string name) override;
        virtual prototype_base deserialize(serializer_view& view) override;
    };

}

//#include <core/serialization/serializer.inl>




