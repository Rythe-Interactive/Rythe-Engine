#include <core/scenemanagement/scene.hpp>
#include <core/serialization/serializationUtil.hpp>


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

    bool SceneManager::createScene(const std::string& name, ecs::entity_handle& ent)
    {
        if (!ent.has_component<scenemanagement::scene>())
        {
            scene s;
            s.id = nameHash(name);
            sceneNames.emplace(s.id, name);
            auto sceneHandle = ent.add_component<scenemanagement::scene>(s);
            sceneList.emplace(nameHash(name), sceneHandle);

            std::ofstream outFile("assets/scenes/"+name+".cornflake");
            serialization::SerializationUtil::JSONSerialize<ecs::entity_handle>(outFile, ent);
            //true if entity does not have the scene component
            return true;
        }
        //false if it doesn't
        return false;
    }

    bool SceneManager::loadScene(const std::string& name)
    {
        if (SceneManager::getScene(name))
        {
            std::ifstream inFile("assets/scenes/" + name + ".cornflake");
            serialization::SerializationUtil::JSONDeserialize<ecs::entity_handle>(inFile);
        }
        else
        {
            std::ifstream inFile("assets/scenes/" + name + ".cornflake");
            serialization::SerializationUtil::JSONDeserialize<ecs::entity_handle>(inFile);
            log::warn("Scene " + name + ".cornflake does not exist in our scenelist, but a file does");
        }
        return true;
    }




    ecs::component_handle<scene> SceneManager::getScene(std::string name)
    {
        return SceneManager::sceneList[nameHash(name)];
    }

    ecs::entity_handle SceneManager::getSceneEntity(std::string name)
    {
        return SceneManager::sceneList[nameHash(name)].entity;
    }
}
