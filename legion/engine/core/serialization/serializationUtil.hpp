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
            log::debug("Started Serializing");
            cereal::JSONOutputArchive archive(os);// Create an output archive, Output as outputing to a filestream
            archive(cereal::make_nvp(typeid(T).name(), serializable)); // Read the data to the archive
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

    template<typename serializableType>
    class DataCache
    {
    public:
        static std::unordered_map<id_type, std::vector<serializableType>> cache;

        static size_t append_list(std::string cacheName, serializableType type)
        {
            id_type id = nameHash(cacheName);
            cache[id].push_back(type);
            return cache[id].size() - 1;
        }

        static serializableType get_item_from_list(std::string cacheName, size_t index)
        {
            id_type id = nameHash(cacheName);
            /* if (!cache[id])
             {
                 log::error("Cache does not exist!"); return nullptr;
             }*/

            if (index < cache[id].size())
            {
                return cache[id][index];
            }
        }

    };

    template <typename T>
    class IniSerializer
    {
    public:

        //ONLY WORKS WITH MATERIAL PARAMS!!!!
        //maybe ill fix this later, idk
        static std::string makeIni(std::string sectionName, T shader)
        {
            int count = 0;
            std::string data = "[" + sectionName + "]\n";
            fs::view outPutPath("assets://textures/testINI/" + sectionName + ".ini");

            auto info = shader.get_uniform_info();
            for (int i = 0; i < info.size(); i++)
            {
                auto [name, location, type] = info[i];
                if (name.substr(0, 3).compare("_L_") != 0)
                {
                    data += std::string(name + "=\n");
                }
            }
            outPutPath.set(fs::basic_resource(std::string_view(data)));
            log::debug(outPutPath.get().decay().to_string());
            return data;
        }

        static std::string setIni(std::string sectionName, T material)
        {
            /* int count = 0;
             std::string data = "[" + sectionName + "]\n";
             fs::view outPutPath("assets://textures/testINI/" + sectionName + ".ini");

             auto info = shader.get_uniform_info();
             for (int i = 0; i < info.size(); i++)
             {
                 auto [name, location, type] = info[i];
                 if (name.substr(0, 3).compare("_L_") != 0)
                 {
                     data += std::string(name + "=\n");
                 }
             }
             outPutPath.set(fs::basic_resource(std::string_view(data)));
             log::debug(outPutPath.get().decay().to_string());*/
            retur "";
        }
    };

    struct cache
    {
    public:
        id_type id;
        std::vector<std::string> data;

        template<typename Archive>
        void serialize(Archive& archive);

        template<typename T>
        void generateCache(T materialParams);

    };

    template<typename Archive>
    inline void cache::serialize(Archive& archive)
    {
        archive(cereal::make_nvp("Name", id), cereal::make_nvp("Data", data));
    }

    template<typename T>
    inline void cache::generateCache(T materialParams)
    {
        
    }
}
