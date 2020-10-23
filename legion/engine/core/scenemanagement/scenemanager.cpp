#include <core/scenemanagement/scenemanager.hpp>
#include <core/scenemanagement/scene.hpp>
namespace legion::core::scenemanagement
{
    std::unordered_map < id_type, std::string> SceneManager::sceneNames;
    std::unordered_map<id_type, ecs::component_handle<scene>> SceneManager::sceneList;



    bool SceneManager::createScene(const std::string& name)
    {
        scene s;
        s.id = nameHash(name);
        sceneNames.emplace(s.id, name);
        auto sceneHandle = m_ecs->createEntity().add_component<scenemanagement::scene>(s);
        sceneList.emplace(nameHash(name), sceneHandle);
        return true;
    }

    bool SceneManager::createScene(const std::string& name, const ecs::entity_handle& ent)
    {
        if (!ent.has_component<scenemanagement::scene>())
        {
            scene s;
            s.id = nameHash(name);
            sceneNames.emplace(s.id, name);
            auto sceneHandle = ent.add_component<scenemanagement::scene>(s);
            sceneList.emplace(nameHash(name), sceneHandle);
            //true if entity does not have the scene component
            return true;
        }
        //false if it doesn't
        return false;
    }



    ecs::component_handle<scene> SceneManager::getScene(std::string name)
    {
        return SceneManager::sceneList[nameHash(name)];
    }
}
