#pragma once
#include <core/engine/module.hpp>
#include <core/defaults/defaultcomponents.hpp>
#include <core/data/importers/mesh_importers.hpp>
#include <core/data/importers/image_importers.hpp>
#include <core/filesystem/provider_registry.hpp>
#include <core/filesystem/basic_resolver.hpp>

#include <core/compute/context.hpp>
#include <core/scenemanagement/scene.hpp>

namespace legion::core
{
    class CoreModule : public Module
    {
    public:
        virtual void setup() override
        {
            filesystem::provider_registry::domain_create_resolver<filesystem::basic_resolver>("assets://", "./assets");
            filesystem::provider_registry::domain_create_resolver<filesystem::basic_resolver>("engine://", "./engine");

            filesystem::AssetImporter::reportConverter<obj_mesh_loader>(".obj");

            for (cstring extension : stb_image_loader::extensions)
                filesystem::AssetImporter::reportConverter<stb_image_loader>(extension);

            log::info("Creating OpenCL");
            compute::Context::init();
            log::info("Done creating OpenCL");

            reportComponentType<position>();
            reportComponentType<rotation>();
            reportComponentType<scale>();
            reportComponentType<scenemanagement::scene>();
            reportSystem <scenemanagement::SceneManager>();
        }

        virtual priority_type priority() override
        {
            return PRIORITY_MAX;
        }

    };
}
