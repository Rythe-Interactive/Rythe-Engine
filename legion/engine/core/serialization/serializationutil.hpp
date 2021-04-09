#pragma once
#include <core/platform/platform.hpp>
#include <core/ecs/handles/component.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>

#include <sstream>
#include <fstream>
#include <string>
#include <memory>
#include <any>



namespace legion::core::serialization
{
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
        virtual std::unique_ptr<component_prototype_base> deserialize(std::string json) LEGION_PURE;
    };

    template<typename type>
    struct serializer : serializer_base
    {
    public:
        serializer() = default;

        virtual std::unique_ptr<component_prototype_base> deserialize(std::string json)
        {
            return json_serializer::deserialize<type>(json);
        }
    };

    struct json_serializer
    {
    public:
        /**@brief JSON serialization to a string
         * @param serializable template type that represents the object that needs to be serialized
         */
        template<typename type>
        static std::string serialize(type& serializable)
        {
            //todo
            return "";
        }

        /**@brief JSON deserialization from a string
         * @param json the input JSON string
         * @returns the the deserialized object as type
         */
        template<typename type>
        static std::unique_ptr<component_prototype<type>> deserialize(std::string json)
        {
            return std::unique_ptr<component_prototype<type>>();
        }
    };

}
