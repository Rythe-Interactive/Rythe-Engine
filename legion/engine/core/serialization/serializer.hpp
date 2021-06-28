#pragma once
#include <core/filesystem/filesystem.hpp>
#include <core/platform/platform.hpp>
#include <core/ecs/handles/component.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>
#include <nlohmann/json.hpp>

#include <sstream>
#include <fstream>
#include <string>
#include <memory>
#include <any>

namespace legion::core::serialization
{
    using json = nlohmann::json;
    //Some testing objects for serialization
#pragma region TestObjects
    struct MyRecord
    {
    public:
        uint8_t x;
        uint8_t y;
        float z;
        MyRecord() = default;
    };
    struct Records
    {
        MyRecord records[20];

    };
#pragma endregion

    struct serializer_base
    {
        virtual ~serializer_base() = default;
    };

    template<typename type>
    struct serializer : public serializer_base
    {
    public:
        json serialize(type data);
        prototype_base deserialize(json j);

        void write(fs::view filePath, type data);
        prototype_base read(fs::view filePath);
    };
}

#include <core/serialization/serializer.inl>


