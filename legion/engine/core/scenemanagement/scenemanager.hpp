#pragma once
#include <core/engine/system.hpp>
#include <core/ecs/handles/component.hpp>
#include <core/filesystem/filesystem.hpp>
#include <core/filesystem/view.hpp>

/**
*@file scenemanager.hpp
*/
namespace legion::core::scenemanagement
{
    struct scene;

    class SceneManager final : public core::System<SceneManager>
    {
    public:
        static int sceneCount;
        static std::string currentScene;
        static std::unordered_map<id_type, std::string> sceneNames;
        static std::unordered_map<id_type, ecs::component<scene>> sceneList;

        SceneManager() = default;

        /**@brief Initialization of the SceneManager
          * @note During the setup we attempt to find all .cornflake files and preload them.
          */
        virtual void setup()
        {
            fs::view fileView = fs::view("assets://scenes/");
            auto files = fileView.ls();
            if (files == common::valid)
            {
                for (auto file : files.value())
                {
                    if (file.get_extension() == common::valid)
                    {
                        if (file.get_extension().value() == ".cornflake")
                        {
                            auto fileName = file.get_filename().value();
                            log::debug("Added {}",fileName);
                            sceneNames.emplace(nameHash(fileName), fileName);
                        }
                    }
                }
            }
        }

        /**@brief Creates a scene entity, to store all objects
         */
        static ecs::entity create_scene_entity();

        /**@brief Creates a scene with given name.
          * @param name The name you wish to set the scene.
          * @returns bool Signifying whether it was successful.
          */
        static bool create_scene(const std::string& name);

        /**@brief Creates a scene with given name.
          * @param name The name you wish to set the scene to.
          * @param ent A specific entity to create a scene from.
          * @returns bool Signifying whether it was successful.
          */
        static bool create_scene(const std::string& name, ecs::entity& ent);

        /**@brief Deserializes the scene from the disk.
         * @param name The name of the file to deserialize.
         * @returns bool Signifying whether it was successful.
         */
        static bool load_scene(const std::string& name);

        /**@brief Serializes a scene to disk
          * @param name string of the name of the scene you wish to save.
          * @param ent a specific entity to serialize.
          * @returns bool Signifying whether it was successful.
         */
        static bool save_scene(const std::string& name, ecs::entity& ent);

        /**@brief Gets a scene from the scene list.
          * @param name The name of the scene that you wish to save.
          * @returns component<scene> The component handle for the scene component stored in the sceneList.
          */
        static ecs::component<scene> get_scene(std::string name);

        /**@brief Gets a scene.
         * @param name string of the scenes name that you wish to get.
         * @returns entiyt_handle The entity handle of the scene.
         */
        static ecs::entity get_scene_entity(std::string name);

       
    };
}
