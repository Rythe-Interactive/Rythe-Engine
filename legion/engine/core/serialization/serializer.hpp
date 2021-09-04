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

        virtual void serialize(const std::any& serializable, serializer_view& view) = 0;
        virtual prototype_base deserialize(serializer_view& view) = 0;
    };


    template<typename serializable_type>
    struct serializer : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual void serialize(const std::any& serializable, serializer_view& view) override;
        virtual prototype_base deserialize(serializer_view& view) override;
    };

    template<>
    struct serializer<std::vector<ecs::entity>> : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        void serialize(std::vector<ecs::entity>&serializable, serializer_view & view);
    };
}

//#include <core/serialization/serializer.inl>




