#pragma once
#include <core/filesystem/filesystem.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>
#include <nlohmann/json.hpp>

#include <sstream>
#include <fstream>

namespace legion::core::serialization
{
    using json = nlohmann::ordered_json;

    enum SerializeFormat
    {
        JSON,
        BINARY,
        YAML
    };

    struct serializer_base
    {
        virtual ~serializer_base() = default;

        //virtual json serialize(const std::any data, SerializeFormat format);
        //virtual prototype_base deserialize(const json j, SerializeFormat format);
    };

    //Serializer should get the type of the thing we are serializing
    template<typename serializable_type>
    struct serializer : public serializer_base
    {
        //returns the serialized json
        json serialize(const serializable_type &data, const SerializeFormat format);

        json serialize_prototype(const serializable_type data, const SerializeFormat format);

        //return prototype of type
        prototype<serializable_type> deserialize(const json j, const SerializeFormat format);

        //writes the given data to a file specified
        void write(const fs::view filePath, const serializable_type &data, const SerializeFormat format);
        //reads the data from the file specified
        serializable_type read(const fs::view filePath, const SerializeFormat format);

        using value_type = serializable_type;
    };
}



