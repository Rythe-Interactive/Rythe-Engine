#pragma once
#include <core/platform/platform.hpp>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <core/filesystem/filesystem.hpp>

#include <sstream>
#include <fstream>
#include <string>
#include <memory>
#include <any>


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
        ar(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z));
    }
};


struct Records
{
    MyRecord records[20];

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(CEREAL_NVP(records));
    }
};
#pragma endregion

namespace legion::core::serialization
{

    class SerializationUtil
    {
    public:
        /**@brief JSON serialization from a stringstream
         * @param os stringstream the ouput for the serialized object
         * @param serializable template type that represents the object that needs to be serialized
         */
        template<class T>
        static void JSONSerialize(std::stringstream os, T serializable)
        {
            cereal::JSONOutputArchive archive(os); // Create an output archive, Output as outputing to a string stream
            archive(CEREAL_NVP(serializable)); // Read the data into the archive
        }

        /**@brief JSON deserialization from a stringstream
         * @param is stringstream the input of a serialized object
         * @returns the the deserialized object as type T
         */
        template<class T>
        static T JSONDeserialize(std::stringstream is)
        {
            cereal::JSONInputArchive iarchive(is); // Create an input archive, Input as inputing to memory
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }

        /**@brief Binary serialization from a stringstream
         * @param os stringstream the ouput for the serialized object
         * @param serializable template type that represents the object that needs to be serialized
         */
        template<class T>
        static void BinarySerialize(std::stringstream os, T serializable)
        {
            cereal::BinaryOutputArchive archive(os); // Create an output archive, Output as outputing to a string stream
            archive(CEREAL_NVP(serializable)); // Read the data into the archive
        }

        /**@brief Binary deserialization from a stringstream
         * @param is stringstream the input of a serialized object
         * @returns the the deserialized object as type T
         */
        template<class T>
        static T BinaryDeserialize(std::stringstream is)
        {
            cereal::BinaryInputArchive iarchive(is);  // Create an input archive, Input as inputing to memory
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }

        /**@brief JSON serialization from a filestream
         * @param os filestream  the ouput for the serialized object
         * @param serializable template type that represents the object that needs to be serialized
         */
        template<class T>
        static void JSONSerialize(std::ofstream& os, T serializable)
        {
            log::debug("[CEREAL] Started Serializing");
            time::timer timer;
            cereal::JSONOutputArchive archive(os);// Create an output archive, Output as outputing to a filestream
            archive(cereal::make_nvp(typeid(T).name(), serializable)); // Read the data to the archive
            log::debug("[CEREAL] Finished Serializing in : {}s", timer.end().seconds());
        }

        /**@brief JSON deserialization from a filestream
         * @param is filestream the input of a serialized object
         * @returns the the deserialized object as type T
         */
        template<class T>
        static T JSONDeserialize(std::ifstream& is)
        {
            cereal::JSONInputArchive iarchive(is);   // Create an input archive, Input as inputing to memory
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }

        /**@brief Binary serialization from a filestream
         * @param os filestream the ouput for the serialized object
         * @param serializable template type that represents the object that needs to be serialized
         */
        template<class T>
        static void BinarySerialize(std::ofstream& os, T serializable)
        {
            cereal::BinaryOutputArchive archive(os);// Create an output archive, Output as outputing to a string stream
            archive(CEREAL_NVP(serializable)); // Read the data to the archive
        }

        /**@brief Binary deserialization from a filestream
         * @param is filestream the input of a serialized object
         * @returns the the deserialized object as type T
         */
        template<class T>
        static T BinaryDeserialize(std::ifstream& is)
        {
            cereal::BinaryInputArchive iarchive(is);  // Create an input archive, Input as inputing to memory
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }
    };
}
