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

    //struct serializer_base
    //{
    //public:
    //    serializer_base() = default;
    //    virtual void read(legion::core::fs::view filePath) LEGION_PURE;
    //    virtual void write(legion::core::fs::view filePath) LEGION_PURE;

    //    virtual void store() LEGION_PURE;
    //    virtual std::unique_ptr<component_prototype_base> load(json j) LEGION_PURE;
    //};

    template<typename type>
    struct serializer/* : serializer_base*/
    {
    private:
        type data;

    public:
        serializer() = default;
        
        void read(fs::view filePath)
        {
            data = json_view::deserialize<type>(filePath);
        }

        void write(fs::view filePath)
        {
            std::ofstream os(filePath.get_virtual_path());
            os << json_view::serialize<type>(data).dump();
        }

        void store(type t)
        {
            data = t;
        }

        std::unique_ptr<component_prototype<type>> load()
        {
            return std::unique_ptr<component_prototype<type>>(ecs::component<type>(data));
        }
    };

    struct json_view
    {
    public:
        /**@brief JSON serialization to a string
         * @param serializable template type that represents the object that needs to be serialized
         */
        template<typename type>
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
        static type deserialize(fs::view filePath)
        {
            type t;
            return t;
        }
    };

}
