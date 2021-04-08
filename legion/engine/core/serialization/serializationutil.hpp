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

        template<typename type>
        component_prototype<type> deserialize(std::string json);
    };

    template<typename type>
    struct serializer : public serializer_base
    {
    public:
        serializer() : serializer_base() {}

        component_prototype<type> deserialize(std::string json)
        {
            return component_prototype<type>(json_serializer::deserialize<type>(json));
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
        static type deserialize(std::string json)
        {
            //todo
            type t;
            log::debug("Deserializeing");
            return t;
        }

        template<class type>
        static type deserialize(std::ifstream& fstream)
        {
            //todo
            type t;
            return t;
        }
    };

}
