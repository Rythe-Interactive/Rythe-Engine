#pragma once
#include <core/filesystem/assetimporter.hpp>
#include <core/data/mesh.hpp>

/**
 * @file mesh_importers.hpp
 */

namespace args::core
{
    /**
     * @class obj_mesh_loader
     * @brief Data converter to be used by ::filesystem::AssetImporter
     * @ref args::core::filesystem::AssetImporter
     * @ref args::core::filesystem::resource_converter
     */
    struct ARGS_API obj_mesh_loader : public filesystem::resource_converter<mesh, mesh_import_settings>
    {
        virtual common::result_decay_more<mesh, fs_error> load(const filesystem::basic_resource& resource, mesh_import_settings&& settings) override;
    };
}
