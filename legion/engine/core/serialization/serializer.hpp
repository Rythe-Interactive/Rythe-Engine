#pragma once
#include <core/filesystem/filesystem.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>
#include <nlohmann/json.hpp>

#include <sstream>
#include <fstream>

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

    //Serializer should get the type of the thing we are serializing
    template<typename type>
    struct serializer : public serializer_base
    {
    public:
        //returns the serialized json
        json serialize(const type data);
        //return prototype of type
        prototype<type> deserialize(const json j);
        //this one is for components
       // prototype<ecs::component<type>> deserialize(const json j,int i = 0);

        //writes the given data to a file specified
        void write(const fs::view filePath, const type data);
        //reads the data from the file specified
        type read(const fs::view filePath);
    };
}

#include <core/serialization/serializer.inl>


