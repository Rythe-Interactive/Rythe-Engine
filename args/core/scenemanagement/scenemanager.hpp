#pragma once
#include <core/engine/system.hpp>
#include <core/scenemanagement/scene.hpp>
#include <core/ecs/component_handle.hpp>

namespace args::core::scenemanagement
{
    class scene;

    class SceneManager final : public core::System<SceneManager>
    {
    public:
        int sceneCount;


        struct ARGS_API static_data {
            static std::unordered_map < id_type, std::string> sceneNames;
            static std::unordered_map < id_type, ecs::component_handle < scene > > sceneList;
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
            scene s;
            s.id = nameHash(name);
            static_data::sceneNames.emplace(s.id,name);
            auto sceneHandle = static_data::registry->createEntity().add_component(s);
            static_data::sceneList.emplace(nameHash(name), sceneHandle);
        }


        static ecs::component_handle<scene> getScene(std::string name)
        {
            return static_data::sceneList[nameHash(name)];
        }

        void loadScene()
        {
        }

        void saveScene()
        {
        }
    };
}
