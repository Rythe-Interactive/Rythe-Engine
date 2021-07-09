#pragma once
#include <core/filesystem/filesystem.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>
#include <nlohmann/json.hpp>

#include <sstream>
#include <fstream>

namespace legion::core::serialization
{
    using json = nlohmann::json;

    template<typename type>
    struct serializer_view : fs::view
    {
    public:
        serializer_view() = default;
        ~serializer_view() = default;
        serializer_view(std::string_view filePath) : fs::view(filePath) {}

    protected:
        virtual json serialize(type object) LEGION_PURE;
        virtual prototype_base deserialize(json j) LEGION_PURE;
    };

    template<typename type>
    struct json_view : public serializer_view<type>
    {
    public:
        json_view() = default;
        ~json_view() = default;
        json_view(std::string_view filePath) : serializer_view<type>(filePath) {}

        virtual json serialize(type object) override;
        virtual prototype_base deserialize(json j) override;
    };
}

#include <core/serialization/view_type.inl>
