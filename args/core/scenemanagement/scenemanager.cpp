#include <core/scenemanagement/scenemanager.hpp>
#include <core/scenemanagement/scene.hpp>
namespace args::core::scenemanagement
{
    std::unordered_map<id_type, args::core::scenemanagement::Scene> SceneManager::static_data::sceneList;
    std::unique_ptr<args::core::scenemanagement::Scene> SceneManager::static_data::scene;
    args::core::ecs::EcsRegistry* SceneManager::static_data::registry;
}
