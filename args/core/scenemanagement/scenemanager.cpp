#include <core/scenemanagement/scenemanager.hpp>
#include <core/scenemanagement/scene.hpp>
namespace args::core::scenemanagement
{
    std::unordered_map < id_type, std::string> SceneManager::static_data::sceneNames;
    std::unordered_map<id_type, ecs::component_handle<scene>> SceneManager::static_data::sceneList;
    ecs::EcsRegistry* SceneManager::static_data::registry;
}
