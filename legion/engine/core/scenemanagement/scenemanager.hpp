#pragma once
#include <core/engine/system.hpp>
#include <core/ecs/component_handle.hpp>
#include <core/filesystem/filesystem.hpp>
#include <core/filesystem/view.hpp>

namespace legion::core::scenemanagement
{
    struct scene;

    class SceneManager final : public core::System<SceneManager>
    {
    public:
        static int sceneCount;
        static std::string currentScene;
        static std::unordered_map < id_type, std::string> sceneNames;
        static std::unordered_map < id_type, ecs::component_handle <scene > > sceneList;
   /*     fs::view fileView;*/

        SceneManager() = default;

        /**@brief Initialization of the SceneManager
          * @note Not used yet.
          */
        virtual void setup()
        {
            fs::view fileView = fs::view("assets://scenes");
            auto files = fileView.ls();
            if (files == common::valid)
            {
                for (auto file : files.decay())
                {
                    log::debug("I am  File");
                    if (file.get_extension() == common::valid)
                    {
                        log::debug("My Extension is legal");
                        log::debug(file.get_extension().decay());
                        if (file.get_extension().decay() == ".cornflake")
                        {
                            auto fileName = file.get_filename().decay();
                            sceneNames.emplace(nameHash(fileName), fileName);
                            log::debug(fileName);
                            std::cout << fileName;
                        }
                    }
                }
            }
            log::debug("This Setup Works");
        }

        /**@brief Updates the SceneManager.
         * @note Not used yet
         */
        void update()
        {

        }

        /**@brief Creates a scene with given name.
          * @param name string of the name you wish to set the scene
          * @returns a bool signifying whether it was successful
          */
        static bool createScene(const std::string& name);

        /**@brief Creates a scene with given name.
          * @param name string of the name you wish to set the scene to
          * @param ent a specific entity to create a scene from
          * @returns a bool signifying whether it was successful
          */
        static bool createScene(const std::string& name, ecs::entity_handle& ent);

        /**@brief Deserializes the scene from the disk
         * @param name of the file to deserialize
         * @note Not yet implemented
         */
        static bool loadScene(const std::string& name);

        /**@brief Serializes a scene to disk
          * @param name: string of the name of the scene you wish to save
          * @param ent: a specific entity to serialize
          * @returns a bool signifying whether it was successful
         */
        static bool saveScene(const std::string& name, ecs::entity_handle& ent);

        /**@brief Gets a scene
          * @param name string of the scenes name that you wish to get
          * @returns the component_handle<scene> for the scene
          */
        static ecs::component_handle<scene> getScene(std::string name);

        /**@brief Gets a scene
         * @param name string of the scenes name that you wish to get
         * @returns the entity_handle of the scene
         */
        static ecs::entity_handle getSceneEntity(std::string name);

    };
}
