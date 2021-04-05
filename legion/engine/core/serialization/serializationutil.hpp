#pragma once
#include <core/platform/platform.hpp>

#include <sstream>
#include <fstream>
#include <string>
#include <memory>


//Some testing objects for serialization
#pragma region TestObjects
struct MyRecord
{

    uint8_t x, y;
    float z;
    MyRecord()
    {

    }

    MyRecord(uint8_t _x, uint8_t _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    template <class Archive>
    void serialize(Archive& ar)
    {
    }
};


struct Records
{
    MyRecord records[20];

    template <class Archive>
    void serialize(Archive& ar)
    {
    }
};
#pragma endregion

namespace legion::core::serialization
{
    struct serializer_base
    {
    public:
        template<typename type>
        std::string serialize(type& serializable)
        {
            //todo
            return "";
        }

        /**@brief JSON deserialization from a string
         * @param json the input JSON string
         * @returns the the deserialized object as type
         */
        template<typename type>
        type deserialize(std::string json)
        {
            //todo
            type t;
            return t;
        }
    };


    struct json_serializer : serializer_base
    {
    public:
        json_serializer() = default;
        /**@brief JSON serialization to a string
         * @param serializable template type that represents the object that needs to be serialized
         */
        template<typename type>
        std::string serialize(type& serializable)
        {
            //todo
            return "";
        }

        /**@brief JSON deserialization from a string
         * @param json the input JSON string
         * @returns the the deserialized object as type
         */
        template<typename type>
        type deserialize(std::string json)
        {
            //todo
            type t;
            return t;
        }

        template<class type>
        type deserialize(std::ifstream& fstream)
        {
            //todo
            type t;
            return t;
        }
    };

    struct binary_serialize : serializer_base
    {
    public:
        binary_serialize() = default;
        /**@brief Binary serialization to a string
        * @param serializable template type that represents the object that needs to be serialized
        */
        template<typename type>
        std::string serialize(type serializable)
        {
            //todo
            return "";
        }

        /**@brief Binary deserialization from a string
         * @param binary the input Binary string
         * @returns the the deserialized object as type
         */
        template<typename  type>
        type deserialize(std::string binary)
        {
            //todo
            type t;
            return t;
        }
    };
}
