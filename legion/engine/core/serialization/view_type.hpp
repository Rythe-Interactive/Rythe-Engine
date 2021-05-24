#pragma once
#include <core/filesystem/filesystem.hpp>
#include <core/ecs/handles/component.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>
#include <nlohmann/json.hpp>

#include <sstream>
#include <fstream>
#include <string>

namespace legion::core::serialization
{
    using json = nlohmann::json;

    struct view_type : fs::view
    {
    public:
        view_type() = default;
        ~view_type() = default;

        view_type(std::string_view filePath) : fs::view(filePath) {}
        static 
    };

    struct json_view : view_type
    {
    public:
        json_view() = default;
        ~json_view() = default;

        json_view(std::string_view filePath) : view_type(filePath) {}

        /**@brief JSON serialization
         * @param TODO
         */
        template<typename type>
        json serialize(type t);

        /**@brief JSON deserialization
         * @param TODO
         * @returns TODO
         */
        template<typename type>
        std::unique_ptr<prototype<type>> deserialize(fs::view filePath);

        template<typename type>
        void store(type t);

        template<typename type>
        type load();
    };
}

#include <core/serialization/view_type.inl>
