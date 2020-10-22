#pragma once
#include <core/engine/system.hpp>
#include <core/ecs/component_handle.hpp>

namespace args::core::scenemanagement
{
    class scene;

    class SceneManager final : public core::System<SceneManager>
    {
    public:
        int sceneCount;
        static std::unordered_map < id_type, std::string> sceneNames;
        static std::unordered_map < id_type, ecs::component_handle <scene > > sceneList;
        static args::core::ecs::EcsRegistry* registry;

        SceneManager() = default;

        virtual void setup()
        {
            registry = m_ecs;
        }

        void update()
        {
            {
                //will probably be checking for scene updates here
            }
        }

        static void createScene(const std::string& name);


        static ecs::component_handle<scene> getScene(std::string name);

        void loadScene()
        {
        }

        void saveScene()
        {
        }
    };
}
