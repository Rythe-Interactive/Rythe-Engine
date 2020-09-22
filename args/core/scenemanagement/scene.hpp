#pragma once
#include <unordered_map>
#include <core/ecs/ecsregistry.hpp>

namespace args::core::scenemanagement
{
    class Scene
    {
    public:
        std::unordered_map<int, std::string> sceneObjects;
        int objectCount = 0;
        args::core::ecs::EcsRegistry* m_registry;
        Scene() = default;
        Scene(args::core::ecs::EcsRegistry* _registry)
        {
            m_registry = _registry;
        };

        template<typename Archive>
        void serialize(Archive& archive)
        {
            args::core::ecs::entity_handle world = m_registry->getEntity(1);
            archive(world.get_child(0));
            //archive(cereal::make_nvp("SceneRoot",sceneObjects));
        }
    };
}
