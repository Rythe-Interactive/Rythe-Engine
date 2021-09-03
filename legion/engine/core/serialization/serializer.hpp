#pragma once
#include <core/filesystem/filesystem.hpp>
#include <core/serialization/serializer_view.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>
#include <nlohmann/json.hpp>

#include <sstream>
#include <fstream>
#include <any>

namespace legion::core::serialization
{
    using json = nlohmann::ordered_json;

    //enum DataFormat
    //{
    //    JSON,
    //    BSON,
    //    YAML
    //};

    struct serializer_base
    {
        serializer_base() = default;
        ~serializer_base() = default;

        virtual void serialize(const std::any& serializable, const serializer_view& view) = 0;
        virtual prototype_base deserialize(const serializer_view& view) = 0;
    };


    template<typename serializable_type>
    struct serializer : serializer_base
    {
        serializer() = default;
        ~serializer() = default;

        virtual void serialize(const std::any& serializable, const serializer_view& view) override;
        virtual prototype_base deserialize(const serializer_view& view) override;
    };
}

#include <core/serialization/serializer.inl>




