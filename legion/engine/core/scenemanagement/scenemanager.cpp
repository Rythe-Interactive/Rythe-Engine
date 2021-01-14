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

    bool SceneManager::createScene(const std::string& name)
    {
        auto testEntity = m_ecs->createEntity();
        serialization::cache stuff;
        stuff.data = std::vector<std::string>{ "Hello","World","I","Am","Rowan Ramsey" };
        testEntity.add_component<serialization::cache>(stuff);

        auto sceneEntity = m_ecs->createEntity();
        testEntity.set_parent(sceneEntity);
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

        if (!SceneManager::getScene(name))
        {
            scene s;
            s.id = nameHash(name);
            sceneNames.emplace(s.id, name);
            sceneEntity.add_component<scenemanagement::scene>(s);
            sceneList.emplace(nameHash(name), sceneEntity);
        }
        return SceneManager::saveScene(name, sceneEntity);
    }

    bool SceneManager::importResource(const std::string& path, bool unique, const std::string& unique_ident)
    {
        std::string key = path;
        if(unique)
            key += unique_ident;

        auto extension = path.substr(path.rfind('.'),std::string::npos);

        //ask the AssetImporter to prefetch the data
        return filesystem::AssetImporter ::prefetch(fs::view("assets://" + path),key);
        
    }



    bool SceneManager::prefetchRecursive(nlohmann::json j, id_type hash)
    {

        // prefetch for all children
        for (auto& [key, value] : j.items())
        {
            if(key.empty() || !(value.is_object() || value.is_array())) continue;
            id_type this_hash = hash_combine(nameHash(key), hash);
            prefetchRecursive(value, this_hash);
        }

        // check if current node has a Filepath attribute
        const auto itr = j.find("Filepath");
        if (itr != j.end())
        {

            //check if current node wants a unique resource
            bool unique = false;
            const auto unique_itr = j.find("unique");

            if (unique_itr != j.end())
            {
                unique = unique_itr->get<bool>();
            }
            const std::string path = itr->get<std::string>();

            log::debug("loading path: {}",path);
            //import 
            if(!importResource(path, unique, std::to_string(hash)))
            {
                log::error("failed to load {}",path);
                return false;
            }
            log::info("successfully loaded {}",path);
        }
        return true;

    }


    bool SceneManager::prefetchResources(fs::view fileView)
    {
        //alias json
        using json = nlohmann::json;


        //load resource
        auto result = fileView.get();
        if (result == common::valid)
        {
            const fs::basic_resource resource = result.decay();

            //load json from resource
            const json j = json::parse(resource.to_string());

            //recursive load all resources
            return prefetchRecursive(j, nameHash("root"));
        }
        return false;
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

        std::string filename = name;
        if (!common::ends_with(filename, ".cornflake")) filename += ".cornflake";

        if(!prefetchResources(fs::view("assets://scenes/" + filename)))
        {
            return false;
        }

        std::ifstream inFile("assets/scenes/" + filename);
        auto sceneEntity = serialization::SerializationUtil::JSONDeserialize<ecs::entity_handle>(inFile);
        SceneManager::currentScene = name;

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
        return SceneManager::sceneList[nameHash(name)];
    }

    ecs::entity_handle SceneManager::getSceneEntity(std::string name)
    {
        return SceneManager::sceneList[nameHash(name)].entity;
    }



}
