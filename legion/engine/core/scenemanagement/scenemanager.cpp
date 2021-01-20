#include <core/scenemanagement/components/scene.hpp>
#include <core/serialization/serializationUtil.hpp>
#include <core/logging/logging.hpp>
#include <core/common/string_extra.hpp>


namespace legion::core::scenemanagement
{
    int SceneManager::sceneCount;
    std::string SceneManager::currentScene = "Main";
    std::unordered_map < id_type, std::string> SceneManager::sceneNames;
    std::unordered_map<id_type, ecs::component_handle<scene>> SceneManager::sceneList;
    std::atomic_bool SceneManager::doNotCreateEntities{false};

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
            if (child == sceneEntity)
                continue;
            child.set_parent(sceneEntity);
        }

        if (!getScene(name))
        {
            scene s;
            s.id = nameHash(name);
            sceneNames.emplace(s.id, name);
            sceneEntity.add_component<scene>(s);
            sceneList.emplace(nameHash(name), sceneEntity);
        }
        return SceneManager::saveScene(name, sceneEntity);
    }

    bool SceneManager::createScene(const std::string& name, ecs::entity_handle& ent)
    {
        if (!ent.has_component<scene>())
        {
            scene s;
            s.id = nameHash(name);
            sceneNames.emplace(s.id, name);
            auto sceneHandle = ent.add_component<scene>(s);
            sceneList.emplace(nameHash(name), sceneHandle);
            sceneCount++;
            //true if entity does not have the scene component
            return saveScene(name, ent);
        }
        //false if it doesn't
        return false;
    }

    bool SceneManager::loadScene(const std::string& name)
    {

        std::string filename = name;
        if (!common::ends_with(filename, ".cornflake")) filename += ".cornflake";

        std::ifstream inFile("assets/scenes/" + filename);

        log::debug("Child Count Before: {}",m_ecs->world.child_count());


        doNotCreateEntities = true;
        m_ecs->getEntityLock().critical_section<async::readwrite_guard>([&]
        {
            while(m_ecs->world.child_count() > 0)
            {
                log::debug("children remaining {}",m_ecs->world.child_count() );
                m_ecs->world.get_child(m_ecs->world.child_count() -1).destroy(true);
            }
        });

        log::debug("Child Count After: {}",m_ecs->world.child_count());

        /*
        for(size_type i = m_ecs->world.child_count(); i != 0; i--)
        {
            m_ecs->world.get_child(i-1).destroy(true);
        }
        */


        auto sceneEntity = serialization::SerializationUtil::JSONDeserialize<ecs::entity_handle>(inFile);
        currentScene = name;

        doNotCreateEntities = true;
        //SceneManager::saveScene(name, sceneEntity);
        //log::debug("........Done saving scene");
        return true;
    }

    bool SceneManager::saveScene(const std::string& name, ecs::entity_handle& ent)
    {
        std::ofstream outFile("assets/scenes/" + name + ".cornflake");
        serialization::SerializationUtil::JSONSerialize<ecs::entity_handle>(outFile, ent);
        return true;
    }

    ecs::component_handle<scene> SceneManager::getScene(std::string name)
    {
        return sceneList[nameHash(name)];
    }

    ecs::entity_handle SceneManager::getSceneEntity(std::string name)
    {
        return sceneList[nameHash(name)].entity;
    }
}
