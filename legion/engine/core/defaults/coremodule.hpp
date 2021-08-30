#pragma once
#include <core/engine/module.hpp>
#include <core/data/loaders/obj_mesh_loader.hpp>
#include <core/data/loaders/gltf_mesh_loader.hpp>
#include <core/data/loaders/stb_image_loader.hpp>
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

            assets::AssetCache<mesh>::addLoader<obj_mesh_loader>();
            assets::AssetCache<mesh>::addLoader<gltf_mesh_loader>();

            assets::AssetCache<image>::addLoader<gltf_faux_image_loader>();
            assets::AssetCache<image>::addLoader<stb_image_loader>();

            compute::Context::init();

            createProcessChain("Update");
        }

        virtual priority_type priority() override
        {
            return PRIORITY_MAX;
        }
    };
}
