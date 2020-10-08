#pragma once
#include <core/platform/platform.hpp>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <core/scenemanagement/scene.hpp>

#include <sstream>
#include <fstream>
#include <string>
#define RAPIDJSON_ENDIAN 0

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


namespace args::core::serialization
{
    template<class T>
    class SerializationUtil
    {
    public:
        static void JSONSerialize(std::stringstream os, T serializable)
        {
            cereal::JSONOutputArchive archive(os);
            archive(CEREAL_NVP(serializable));
        }

        static T JSONDeserialize(std::stringstream is)
        {
            cereal::JSONInputArchive iarchive(is); // Create an input archive
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }

        static void BinarySerialize(std::stringstream os, T serializable)
        {
            cereal::BinaryOutputArchive archive(os);
            archive(CEREAL_NVP(serializable));
        }

        static T BinaryDeserialize(std::stringstream is)
        {
            cereal::BinaryInputArchive iarchive(is); // Create an input archive
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }

        static void JSONSerialize(std::ofstream& os, T serializable)
        {
            cereal::JSONOutputArchive archive(os);
            if (std::is_same<T, ecs::component_handle<scenemanagement::scene>>::value)
            {
                archive(cereal::make_nvp("SceneRoot", serializable));
            }
            else
            {
                archive(cereal::make_nvp("Entity", serializable));
            }
        }

        static T JSONDeserialize(std::ifstream is)
        {
            cereal::JSONInputArchive iarchive(is); // Create an input archive
            T t;
            iarchive(t); // Read the data from the archive
            return t;
        }


        static void BinarySerialize(std::ofstream os, T serializable)
        {
            cereal::BinaryOutputArchive archive(os);
            archive(CEREAL_NVP(serializable));
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
