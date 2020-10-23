#pragma once
#include <core/engine/system.hpp>
#include <core/ecs/component_handle.hpp>

namespace legion::core::scenemanagement
{
    class scene;

    class SceneManager final : public core::System<SceneManager>
    {
    public:
        int sceneCount;
        static std::unordered_map < id_type, std::string> sceneNames;
        static std::unordered_map < id_type, ecs::component_handle <scene > > sceneList;

        SceneManager() = default;

        virtual void setup()
        {
        }

        void update()
        {
            {
                //will probably be checking for scene updates here
            }
        }

        static bool createScene(const std::string& name);
        static bool createScene(const std::string& name,const ecs::entity_handle& ent);


        static ecs::component_handle<scene> getScene(std::string name);

        void loadScene()
        {
        }

        void saveScene()
        {
        }
    };
}
