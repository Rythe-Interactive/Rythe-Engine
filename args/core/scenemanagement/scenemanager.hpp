#pragma once
#include <core/engine/system.hpp>
#include <core/scenemanagement/scene.hpp>

namespace args::core::scenemanagement
{
    class Scene;

    class SceneManager final : public core::System<args::core::scenemanagement::SceneManager>
    {
    public:
        int sceneCount;


        struct ARGS_API static_data {
            static std::unordered_map<id_type, args::core::scenemanagement::Scene> sceneList;
            static std::unique_ptr<args::core::scenemanagement::Scene> scene;
            static args::core::ecs::EcsRegistry* registry;

        };
       
        SceneManager() = default;

        virtual void setup()
        {
            static_data::registry = m_ecs;
        }

        void update()
        {
            {
                //will probably be checking for scene updates here
            }
        }


        static void createScene(const std::string& name)
        {
            static_data::sceneList.emplace(nameHash(name), args::core::scenemanagement::Scene(static_data::registry));
        }


        static args::core::scenemanagement::Scene* getScene(std::string name)
        {
            return &(static_data::sceneList[nameHash(name)]);
        }

        void loadScene()
        {
        }

        void saveScene()
        {
        }
    };
}
