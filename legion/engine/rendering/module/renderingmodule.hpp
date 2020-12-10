#pragma once
#include <rendering/data/importers/texture_importers.hpp>
#include <rendering/systems/renderer.hpp>
#include <rendering/systems/particle_system_manager.hpp>
#include <rendering/components/point_cloud.hpp>
#include <rendering/systems/pointcloudgeneration.hpp>
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
            reportComponentType<mesh_renderer>();
            reportComponentType<light>();
            reportSystem<Renderer>();

            reportComponentType<particle>();
            reportComponentType<particle_emitter>();
            reportSystem<ParticleSystemManager>();

            reportComponentType<point_cloud>();
            reportSystem<PointCloudGeneration>();

        }

        virtual priority_type priority() override
        {
            return 99;
        }

    };
}
