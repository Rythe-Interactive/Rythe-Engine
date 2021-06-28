#include <core/scenemanagement/components/scene.hpp>
//#include <core/serialization/serializationutil.hpp>
#include <core/logging/logging.hpp>
#include <core/common/string_extra.hpp>
//#include <core/defaults/defaultcomponents.hpp>
//#include <rendering/components/camera.hpp>


namespace legion::core::scenemanagement
{
    int SceneManager::sceneCount;
    std::string SceneManager::currentScene = "Main";
    std::unordered_map < id_type, std::string> SceneManager::sceneNames;
    std::unordered_map<id_type, ecs::component<scene>> SceneManager::sceneList;

    ecs::entity SceneManager::create_scene_entity()
    {
        ecs::entity sceneEntity;
        //static ecs::filter sceneEntities = m_ecs->createQuery<scenemanagement::scene>();
        //sceneEntities.queryEntities();
        //if (sceneEntities.size() == 0)
        //{
        //    log::debug("Creating a Scene Entity");
        //sceneEntity = m_ecs->createEntity();
        //    sceneEntity.add_component<hierarchy>();
        //    sceneEntity.add_component<scene>();
        //    std::vector<ecs::entity> children;
        //    auto hry = world.read_component<hierarchy>();

        //    for (auto& child : hry.children)
        //    {
        //        children.push_back(child);
        //    }
        //    int i = 0;
        //    sceneEntity.write_component(hry);

        //    for (ecs::entity child : children)
        //    {
        //        if (child.has_component<hierarchy>())
        //        {
        //            auto h = child.read_component<hierarchy>();
        //            h.parent = sceneEntity;
        //            child.write_component(h);
        //        }
        //    }
        //    hry.children.clear();
        //    hry.children.insert(sceneEntity);
        //    world.write_component(hry);
        //}
        //else
        //{
        //    sceneEntity = sceneEntities[0];
        //}
        return sceneEntity;
    }

    bool SceneManager::create_scene(const std::string& name)
    {
        ecs::entity sceneEntity = create_scene_entity();

        if (!get_scene(name))
        {
            scene s;
            s.id = nameHash(name);
            //sceneNames.emplace(s.id, name);
            //sceneList.emplace(nameHash(name), sceneEntity);
        }
        return SceneManager::save_scene(name, sceneEntity);
    }

    bool SceneManager::create_scene(const std::string& name, ecs::entity& ent)
    {
        if (!ent.has_component<scene>())
        {
            scene s;
            s.id = nameHash(name);
            sceneNames.emplace(s.id, name);
            //auto sceneHandle = ent.add_component<scene>(s);
            //sceneList.emplace(nameHash(name), sceneHandle);
            sceneCount++;
            //true if entity does not have the scene component
            return save_scene(name, ent);
        }
        //false if it doesn't
        return false;
    }

    bool SceneManager::load_scene(L_MAYBEUNUSED const std::string& name)
    {
        //std::string filename = name;
        //if (!common::ends_with(filename, ".cornflake")) filename += ".cornflake";

        //std::ifstream inFile("assets/scenes/" + filename);

        //auto hry = world.read_component<hierarchy>();
        //log::debug("Child Count Before: {}", hry.children.size());
        //for (auto child : hry.children)
        //{
        //    log::debug("children remaining {}", world.child_count());
        //    child.destroy(true);
        //}
        //hry.children.clear();
        //world.write_component(hry);
        //log::debug("Child Count After: {}", world.child_count());

        //auto sceneEntity = serialization::serializer_base::deserialize<ecs::entity>(inFile);
        //currentScene = name;

        //SceneManager::saveScene(name, sceneEntity);
        //log::debug("........Done saving scene");
        return true;
    }

    bool SceneManager::save_scene(L_MAYBEUNUSED const std::string& name, L_MAYBEUNUSED ecs::entity& ent)
    {
        //std::ofstream outFile("assets/scenes/" + name + ".cornflake");
        //serialization::serializer_base::serialize<ecs::entity>(outFile, ent);
        return true;
    }

    ecs::component<scene> SceneManager::get_scene(std::string name)
    {
        return sceneList[nameHash(name)];
    }

    ecs::entity SceneManager::get_scene_entity(std::string name)
    {
        return sceneList[nameHash(name)].owner;
    }
}
