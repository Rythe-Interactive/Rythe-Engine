#pragma once
#include <core/engine/system.hpp>
#include <core/scenemanagement/scenemanager.hpp>

namespace legion::core::scenemanagement
{
    struct scene
    {
    public:
        id_type id = invalid_id;

        template<typename Archive>
        void serialize(Archive& archive);
    };

    template<typename Archive>
    inline void scene::serialize(Archive& archive)
    {
        std::string name = SceneManager::sceneNames[id];
        archive(cereal::make_nvp("NAME", name));
        id = nameHash(name);
        SceneManager::sceneNames[id] = name;
    }
}
