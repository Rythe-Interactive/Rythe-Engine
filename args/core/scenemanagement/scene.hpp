#pragma once
#include <core/scenemanagement/scenemanager.hpp>

namespace args::core::scenemanagement
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
        archive(cereal::make_nvp("NAME",SceneManager::static_data::sceneNames[id]));
    }
}
