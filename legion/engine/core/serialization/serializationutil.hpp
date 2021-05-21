#pragma once
#include <core/filesystem/filesystem.hpp>
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
        virtual ~serializer_base() = default;
    };

    template<typename type>
    struct serializer : public serializer_base
    {
    private:
        type data;
    public:
        void read(fs::view filePath);
        void write(fs::view filePath);
        void store(const type& t);
        std::unique_ptr<prototype_base> load();
    };

    struct json_view
    {
    public:
        /**@brief JSON serialization to a string
         * @param serializable template type that represents the object that needs to be serialized
         */
        template<typename type>
        static json serialize(type t);

        /**@brief JSON deserialization from a string
         * @param json the input JSON string
         * @returns the the deserialized object as type
         */
        template<typename type>
        static type deserialize(fs::view filePath);
    };
}

#include <core/serialization/serializationutil.inl>


