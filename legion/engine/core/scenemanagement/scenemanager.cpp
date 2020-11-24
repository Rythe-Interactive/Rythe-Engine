#include <core/scenemanagement/scene.hpp>
#include <core/serialization/serializationUtil.hpp>


namespace legion::core::scenemanagement
{
    int SceneManager::sceneCount;
    std::string SceneManager::currentScene = "Main";
    std::unordered_map < id_type, std::string> SceneManager::sceneNames;
    std::unordered_map<id_type, ecs::component_handle<scene>> SceneManager::sceneList;

    bool SceneManager::createScene(const std::string& name)
    {
        auto sceneEntity = m_ecs->createEntity();
        std::vector<ecs::entity_handle> children;
        for (size_type i = 0; i < m_ecs->world.child_count(); i++)
        {
            children.push_back(m_ecs->world.get_child(i));
        }
        for (auto child : children)
        {
            if (child != sceneEntity)
            {
                child.set_parent(sceneEntity);
            }
        }

        scene s;
        s.id = nameHash(name);
        sceneNames.emplace(s.id, name);
        sceneEntity.add_component<scenemanagement::scene>(s);
        sceneList.emplace(nameHash(name), sceneEntity);
        return SceneManager::saveScene(name, sceneEntity);
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

            SceneManager::sceneCount++;
            //true if entity does not have the scene component
            return SceneManager::saveScene(name, ent);
        }
        //false if it doesn't
        return false;
    }

    bool SceneManager::loadScene(const std::string& name)
    {
        if (SceneManager::getScene(name))
        {
            log::debug("Starting destruction of scene");
            m_ecs->world.get_child(0).destroy();
            log::debug("Done destroying!");

            scene s;
            s.id = nameHash(name);
            sceneNames.emplace(s.id, name);
            auto sceneEntity = m_ecs->createEntity();
            sceneEntity.add_component<scenemanagement::scene>(s);
            m_ecs->world.add_child(sceneEntity);

            std::ifstream inFile("assets/scenes/" + name + ".cornflake");
            serialization::SerializationUtil::JSONDeserialize<ecs::entity_handle>(inFile);
            SceneManager::currentScene = name;
            return true;
        }
        else
        {
            log::debug("Starting destruction of scene");
            m_ecs->world.get_child(0).destroy();
            log::debug("Done destroying!");

            scene s;
            s.id = nameHash(name);
            sceneNames.emplace(s.id, name);
            auto sceneEntity = m_ecs->createEntity();
            sceneEntity.add_component<scenemanagement::scene>(s);
            m_ecs->world.add_child(sceneEntity);

           /* std::ifstream inFile("assets/scenes/" + name + ".cornflake");
            serialization::SerializationUtil::JSONDeserialize<ecs::entity_handle>(inFile);
            log::debug("Scene " + name + ".cornflake does not exist in our scenelist, but a file does");*/
            SceneManager::currentScene = name;

            return true;
        }
        return false;
    }

    bool SceneManager::saveScene(const std::string& name, ecs::entity_handle& ent)
    {
        std::ofstream outFile("assets/scenes/" + name + ".cornflake");
        serialization::SerializationUtil::JSONSerialize<ecs::entity_handle>(outFile, ent);
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
