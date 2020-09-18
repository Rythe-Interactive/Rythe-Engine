#pragma once
#include <core/core.hpp>
#include <core/scene management/scene.hpp>

namespace args::scenemanagement
{
    class ARGS_API SceneManager final: public System<SceneManager>
    {
    public:
        int sceneCount;
        static std::unordered_map<std::string, std::unique_ptr<args::scenemanagement::Scene>> sceneList;
        static std::unique_ptr<args::scenemanagement::Scene> scene;
        static args::ecs::EcsRegistry* registry;

        SceneManager()
        {
            
        }

        virtual void setup()
        {
            registry = m_ecs;
        }

        void update()
        {
            //will probably be checking for scene updates here
        }

        static std::unique_ptr<args::scenemanagement::Scene> createScene(std::string name)
        {
            scene = std::unique_ptr<args::scenemanagement::Scene>(new args::scenemanagement::Scene(*registry));
            sceneList.insert_or_assign(name, scene);
            return sceneList[name];
        }

        void loadScene()
        {

        }

        void saveScene()
        {

        }
    };
}
