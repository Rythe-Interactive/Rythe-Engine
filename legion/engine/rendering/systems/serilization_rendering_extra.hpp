#pragma once
#include <core/engine/system.hpp>
#include <core/scenemanagement/scenemanager.hpp>
#include <rendering/util/additional_material_loader.hpp>

namespace legion::rendering
{
    class SerializationRenderingExtra : public core::System<SerializationRenderingExtra>
    {
        void setup() override
        {
            using scenemanagement::SceneManager;
            SceneManager::add_loader<use_embedded_material>(SceneManager::additional_loader_fn::create<&EmbeddedLoader::loader>()); 
        }  
    };
}
