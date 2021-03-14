#include <core/scenemanagement/components/scene.hpp>
#include <core/serialization/serializationUtil.hpp>
#include <core/logging/logging.hpp>
#include <core/common/string_extra.hpp>
#include <core/defaults/defaultcomponents.hpp>
//#include <rendering/components/camera.hpp>


namespace legion::core::scenemanagement
{
    ecs::EcsRegistry* SceneManager::m_ecs;
    int SceneManager::sceneCount;
    ecs::component_handle<scene> SceneManager::currentScene;
    std::unordered_map<id_type, std::string> SceneManager::sceneNames;
    std::unordered_map<id_type, ecs::component_handle<scene>> SceneManager::sceneList;
    std::unordered_map<id_type, SceneManager::additional_loader_fn> SceneManager::m_additionalLoaders;

    ecs::entity_handle SceneManager::create_scene_entity(const std::string& name)
    {
        ecs::entity_handle sceneEntity = get_scene_entity(name);

        if (!sceneEntity)
        {
            log::debug("Creating a Scene Entity");
            sceneEntity = m_ecs->createEntity();
            sceneEntity.add_component<scene>();
            auto hry = world.read_component<hierarchy>();
            auto worldHry = hry;
            hry.name = name;
            hry.children.erase(sceneEntity);

            for (ecs::entity_handle child : hry.children)
            {
                if (child.has_component<hierarchy>())
                {
                    auto h = child.read_component<hierarchy>();
                    h.parent = sceneEntity;
                    child.write_component(h);
                }
            }

            sceneEntity.add_component(hry);

            worldHry.children.clear();
            worldHry.children.insert(sceneEntity);
            world.write_component(worldHry);
        }
        return sceneEntity;
    }

    ecs::component_handle<scene> SceneManager::create_scene(const std::string& name)
    {
        ecs::entity_handle sceneEntity = create_scene_entity(name);

        if (!get_scene_entity(name))
        {
            scene s = sceneEntity.read_component<scene>();
            s.id = nameHash(name);
            sceneNames.emplace(s.id, name);
            sceneEntity.write_component<scene>(s);
            sceneList.emplace(nameHash(name), sceneEntity.get_component_handle<scene>());
            sceneCount++;
        }
        return SceneManager::save_scene(name, sceneEntity);
    }

    ecs::component_handle<scene> SceneManager::create_scene(const std::string& name, ecs::entity_handle& ent)
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
            return save_scene(name, ent);
        }
        else
        {
            auto s = ent.read_component<scene>();
            if (s.id != nameHash(name))
            {
                s.id = nameHash(name);
                sceneNames.emplace(s.id, name);
                sceneList.emplace(s.id, ent.get_component_handle<scene>());
                sceneCount++;
                ent.set_name(name);
            }
            return save_scene(name, ent);
        }
    }

    ecs::component_handle<scene> SceneManager::load_scene(const std::string& name)
    {
        std::string filename = name;
        if (!common::ends_with(filename, ".cornflake")) filename += ".cornflake";

        std::ifstream inFile("assets/scenes/" + filename);

        auto hry = world.read_component<hierarchy>();
        log::debug("Child Count Before: {}", hry.children.size());
        for (auto child : hry.children)
        {
            log::debug("children remaining {}", world.child_count());
            child.destroy(true);
        }
        hry.children.clear();
        world.write_component(hry);
        log::debug("Child Count After: {}", world.child_count());

        auto sceneEntity = serialization::SerializationUtil::JSONDeserialize<ecs::entity_handle>(inFile);
        currentScene = sceneEntity.get_component_handle<scene>();

        for (auto& [id, fn] : m_additionalLoaders)
        {
            hashed_sparse_set<id_type> types;
            types.insert(id);
            auto query = m_ecs->createQuery(types);
            query.queryEntities();
            for (const auto& child : query)
            {
                fn(child);
            }
        }


        static auto sceneQuery = m_ecs->createQuery<scene>();
        sceneQuery.queryEntities();

        for (auto ent : sceneQuery)
        {
            auto sceneHandle = ent.get_component_handle<scene>();
            auto s = sceneHandle.read();

            sceneList[s.id] = sceneHandle;
        }

        //SceneManager::saveScene(name, sceneEntity);
        //log::debug("........Done saving scene");
        return sceneEntity.get_component_handle<scene>();
    }

    ecs::component_handle<scene> SceneManager::save_scene(const std::string& name, ecs::entity_handle& ent)
    {
        std::ofstream outFile("assets/scenes/" + name + ".cornflake");
        outFile.clear();
        serialization::SerializationUtil::JSONSerialize<ecs::entity_handle>(outFile, ent);
        outFile.close();
        return ent.get_component_handle<scene>();
    }

    ecs::component_handle<scene> SceneManager::get_scene(std::string name)
    {
        return sceneList[nameHash(name)];
    }

    ecs::entity_handle SceneManager::get_scene_entity(std::string name)
    {
        return sceneList[nameHash(name)].entity;
    }
}
