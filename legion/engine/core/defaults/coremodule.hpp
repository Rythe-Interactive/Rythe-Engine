#pragma once
#include <core/engine/module.hpp>
#include <core/data/importers/mesh_importers.hpp>
#include <core/data/importers/image_importers.hpp>
#include <core/filesystem/provider_registry.hpp>
#include <core/filesystem/basic_resolver.hpp>
#include <core/compute/context.hpp>

/**
 * @file coremodule.hpp
 */
namespace legion::core
{
    /**@class CoreModule
     * @brief Custom module.
     */
    class CoreModule final : public Module
    {
    public:
        virtual void setup()
        {
            filesystem::provider_registry::domain_create_resolver<filesystem::basic_resolver>("assets://", "./assets");
            filesystem::provider_registry::domain_create_resolver<filesystem::basic_resolver>("engine://", "./engine");

            filesystem::AssetImporter::reportConverter<obj_mesh_loader>(".obj");
            filesystem::AssetImporter::reportConverter<gltf_binary_mesh_loader>(".glb");
            filesystem::AssetImporter::reportConverter<gltf_ascii_mesh_loader>(".gltf");

            for (cstring extension : stb_image_loader::extensions)
                filesystem::AssetImporter::reportConverter<stb_image_loader>(extension);

            compute::Context::init();

            createProcessChain("Update");
        }

        virtual priority_type priority() override
        {
            return PRIORITY_MAX;
        }
    };
}
