#pragma once

#include <core/filesystem/assetimporter.hpp>
#include <core/data/mesh.hpp>

/**
 * @file mesh_importers.hpp
 */

namespace legion::core
{
    /**
     * @class obj_mesh_loader
     * @brief Data converter to be used by ::filesystem::AssetImporter
     * @ref legion::core::filesystem::AssetImporter
     * @ref legion::core::filesystem::resource_converter
     */
    struct obj_mesh_loader : public filesystem::resource_converter<mesh, mesh_import_settings>
    {
        common::result<mesh, fs_error> load_default(const filesystem::basic_resource& resource) override
        {
            return load(resource, mesh_import_settings(default_mesh_settings));
        }
        virtual common::result<mesh, fs_error> load(const filesystem::basic_resource& resource, mesh_import_settings&& settings) override;
    };

    class gltf_faux_image_loader : public assets::AssetLoader<image>
    {
    public:
        using base = assets::AssetLoader<image>;
        using base::asset_ptr;
        using base::import_cfg;
        using base::progress_type;

        virtual bool canLoad(const fs::view& file) override { return false; }

        virtual common::result<asset_ptr> load(id_type nameHash, const std::string& name, const fs::view& file, const import_cfg& settings) override { return legion_exception_msg("Tried to use a faux loader"); }

        virtual void free(image& asset) override { delete[] asset.data(); }
    };

    /**
     * @class gltf_binary_mesh_loader
     * @brief Data converter for .glb files (gltf binary format), used by ::filesystem::AssetImporter
     * @ref legion::core::filesystem::AssetImporter
     * @ref legion::core::filesystem::resource_converter
     */
    struct gltf_binary_mesh_loader : public filesystem::resource_converter<mesh, mesh_import_settings>
    {
        common::result<mesh, fs_error> load_default(const filesystem::basic_resource& resource) override
        {
            return load(resource, mesh_import_settings(default_mesh_settings));
        }
        virtual common::result<mesh, fs_error> load(const filesystem::basic_resource& resource, mesh_import_settings&& settings) override;
    };

    /**
     * @class gltf_ascii_mesh_loader
     * @brief Data converter for .gltf files (gltf ascii format), used by ::filesystem::AssetImporter
     * @ref legion::core::filesystem::AssetImporter
     * @ref legion::core::filesystem::resource_converter
     */
    struct gltf_ascii_mesh_loader : public filesystem::resource_converter<mesh, mesh_import_settings>
    {
        common::result<mesh, fs_error> load_default(const filesystem::basic_resource& resource) override
        {
            return load(resource, mesh_import_settings(default_mesh_settings));
        }
        virtual common::result<mesh, fs_error> load(const filesystem::basic_resource& resource, mesh_import_settings&& settings) override;
    };
}
