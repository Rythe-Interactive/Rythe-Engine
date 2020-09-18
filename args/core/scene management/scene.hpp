#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <unordered_map>
#include <string>
#include <memory>

namespace args::scenemanagement
{
    class Scene
    {
    public:
        std::unordered_map<int, std::string> sceneObjects;
        int objectCount = 0;
        args::core::ecs::EcsRegistry* m_registry;

        Scene(args::core::ecs::EcsRegistry& _registry)
        {
            m_registry = &_registry;
        };

        template<typename Archive>
        void serialize(Archive& archive)
        {
            args::ecs::entity_handle world = m_registry.getEntity(1);
            archive(world.get_child(0));
            //archive(cereal::make_nvp("SceneRoot",sceneObjects));
        }
    };
}
