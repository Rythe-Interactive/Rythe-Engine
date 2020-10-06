#include <core/scenemanagement/scenemanager.hpp>
#include <core/scenemanagement/scene.hpp>
namespace args::core::scenemanagement
{
    std::unordered_map < id_type, std::string> SceneManager::sceneNames;
    std::unordered_map<id_type, ecs::component_handle<scene>> SceneManager::sceneList;
    ecs::EcsRegistry* SceneManager::registry;



    void SceneManager::createScene(const std::string& name)
    {
        scene s;
        s.id = nameHash(name);
        sceneNames.emplace(s.id, name);
        auto sceneHandle = registry->createEntity().add_component(s);
        sceneList.emplace(nameHash(name), sceneHandle);
    }


    ecs::component_handle<scene> SceneManager::getScene(std::string name)
    {
        return SceneManager::sceneList[nameHash(name)];
    }
}
