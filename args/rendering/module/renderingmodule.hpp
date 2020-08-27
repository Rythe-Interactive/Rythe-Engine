#pragma once
#include <rendering/data/importers/mesh_importers.hpp>

namespace args::rendering
{
    class RenderingModule : public Module
    {
    public:
        virtual void setup() override
        {
            fs::AssetImporter::reportConverter<obj_mesh_loader>(".obj");
        }

        virtual priority_type priority() override
        {
            return 9;
        }

    };
}
