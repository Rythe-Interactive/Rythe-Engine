#pragma once
#include <rendering/data/importers/texture_importers.hpp>
#include <rendering/systems/renderer.hpp>
#include <rendering/systems/particle_system_manager.hpp>


namespace legion::rendering
{
    class RenderingModule : public Module
    {
    public:
        virtual void setup() override
        {
            for (cstring extension : stbi_texture_loader::extensions)
                fs::AssetImporter::reportConverter<stbi_texture_loader>(extension);

            reportComponentType<camera>();
            reportComponentType<renderable>();
            reportComponentType<light>();
            reportSystem<Renderer>();

            reportComponentType<particle>();
            reportComponentType<particle_emitter>();
            reportSystem<ParticleSystemManager>();
        }

        virtual priority_type priority() override
        {
            return 99;
        }

    };
}
