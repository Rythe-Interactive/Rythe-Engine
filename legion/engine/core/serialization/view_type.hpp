#pragma once
#include <core/filesystem/filesystem.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>
#include <core/ecs/handles/entity.hpp>

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
        virtual type deserialize(json j) LEGION_PURE;
    };

    template<typename prototype>
    struct json_view : public serializer_view<prototype>
    {
    public:
        json_view() = default;
        ~json_view() = default;
        json_view(std::string_view filePath) : serializer_view<prototype>(filePath) {}

        virtual json serialize(const prototype object) override;
        virtual prototype deserialize(const json j) override;
    };

    struct serialization_util
    {
        template<typename property_type>
        static json serialize_property(const property_type prop);

        template<typename property_type>
        static property_type deserialize_property(const json j);

        template<typename container_type>
        static json serialize_container(const container_type prop);

        template<typename container_type>
        static container_type deserialize_container(const json j);
    };

}

#include <core/serialization/view_type.inl>
