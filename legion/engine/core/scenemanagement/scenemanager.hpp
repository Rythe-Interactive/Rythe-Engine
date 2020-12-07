#pragma once
#include <core/engine/system.hpp>
#include <core/ecs/component_handle.hpp>

namespace legion::core::scenemanagement
{
    struct scene;

    class SceneManager final : public core::System<SceneManager>
    {
    public:
        int sceneCount;
        static std::unordered_map < id_type, std::string> sceneNames;
        static std::unordered_map < id_type, ecs::component_handle <scene > > sceneList;

        SceneManager() = default;

        /**@brief Initialization of the SceneManager
          * @note Not used yet.
          */
        virtual void setup()
        {

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



        /**@brief Serializes a scene to disk
          * @param .name of the file to save as, default is the scenes name
          * @note Not yet implemented
          */
        void saveScene()
        {

        }
    };
}
