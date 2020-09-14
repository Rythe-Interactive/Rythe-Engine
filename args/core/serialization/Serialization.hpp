#pragma once
#include <core/platform/platform.hpp>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <fstream>
#include <string>

struct MyRecord
{
    uint8_t x, y;
    float z;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z));
    }
};

namespace args::serialization
{
    template<class T>
    class SerializationUtil
    {
    public:

        static void serialize(std::ofstream os, T component)
        {
            cereal::BinaryOutputArchive archive(os);
            archive(CEREAL_NVP(component));
        }

        static T deserialize(std::ifstream is)
        {
            cereal::BinaryInputArchive iarchive(is); // Create an input archive
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }
    };
}
