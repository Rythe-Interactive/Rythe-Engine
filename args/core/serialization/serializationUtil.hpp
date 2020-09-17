#pragma once
#include <core/platform/platform.hpp>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <sstream>
#include <fstream>
#include <string>

struct MyRecord
{
    uint8_t x, y;
    float z;
    MyRecord()
    {

    }

    MyRecord(uint8_t _x,uint8_t _y, float _z)
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


namespace args::serialization
{
    template<class T>
    class SerializationUtil
    {
    public:
        static void JSONSerialize(std::stringstream os, T component)
        {
            cereal::JSONOutputArchive archive(os);
            archive(CEREAL_NVP(component));
        }

        static T JSONDeserialize(std::stringstream is)
        {
            cereal::JSONInputArchive iarchive(is); // Create an input archive
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }

        static void BinarySerialize(std::stringstream os, T component)
        {
            cereal::BinaryOutputArchive archive(os);
            archive(CEREAL_NVP(component));
        }

        static T BinaryDeserialize(std::stringstream is)
        {
            cereal::BinaryInputArchive iarchive(is); // Create an input archive
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }

        static void JSONSerialize(std::ofstream os, T component)
        {
            cereal::JSONOutputArchive archive(os);
            archive(CEREAL_NVP(component));
            //return archive;
        }

        static T JSONDeserialize(std::ifstream is)
        {
            cereal::JSONInputArchive iarchive(is); // Create an input archive
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }


        static void BinarySerialize(std::ofstream os, T component)
        {
            cereal::BinaryOutputArchive archive(os);
            archive(CEREAL_NVP(component));
        }

        static T BinaryDeserialize(std::ifstream is)
        {
            cereal::BinaryInputArchive iarchive(is); // Create an input archive
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }
    };
}
