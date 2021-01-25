#pragma once
#include <core/engine/system.hpp>
#include <core/ecs/component_handle.hpp>
#include <core/scenemanagement/scenemanager.hpp>

namespace legion::core::scenemanagement
{
    struct scene
    {
    public:
        id_type id;

        template<typename Archive>
        void serialize(Archive& archive);
    };

    template<typename Archive>
    inline void scene::serialize(Archive& archive)
    {
        archive(cereal::make_nvp("NAME",SceneManager::sceneNames[id]));
    }
}
