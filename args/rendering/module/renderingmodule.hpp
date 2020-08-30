#pragma once
#include <rendering/data/importers/mesh_importers.hpp>
#include <rendering/data/importers/texture_importers.hpp>
#include <rendering/systems/renderer.hpp>

namespace args::rendering
{
    class RenderingModule : public Module
    {
    public:
        virtual void setup() override
        {
            fs::AssetImporter::reportConverter<obj_mesh_loader>(".obj");

            for (cstring extension : stbi_texture_loader::extensions)
                fs::AssetImporter::reportConverter<stbi_texture_loader>(extension);

            fs::AssetImporter a;
            a.foo();

            reportComponentType<camera>();
            reportComponentType<renderable>();
            reportSystem<Renderer>();
        }

        virtual priority_type priority() override
        {
            return 99;
        }

    };
}
