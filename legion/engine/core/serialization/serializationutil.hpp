#pragma once
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
    public:
        serializer_base() = default;
        virtual std::unique_ptr<component_prototype_base> deserialize(json j) LEGION_PURE;
    };

    template<typename type>
    struct serializer : serializer_base
    {
    public:
        serializer() = default;
        virtual std::unique_ptr<component_prototype_base> deserialize(json j)
        {
            return json_serializer::deserialize<type>(j);
        }
    };

    struct json_serializer
    {
    public:
        /**@brief JSON serialization to a string
         * @param serializable template type that represents the object that needs to be serialized
         */
        template<size_t I = 0, typename type>
        static json serialize(type t)
        {
            component_prototype<type> temp = component_prototype<type>(t);
            json j;
            j["Type ID"] = type_hash<type>().local();
            return j;
        }

        /**@brief JSON deserialization from a string
         * @param json the input JSON string
         * @returns the the deserialized object as type
         */
        template<typename type>
        static std::unique_ptr<component_prototype<type>> deserialize(json j)
        {
            component_prototype<type> prototype(out_type);
            return std::make_unique<component_prototype<type>>(prototype);
        }
    };

}
