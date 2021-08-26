#pragma once
#include <core/assets/assets.hpp>
#include <core/data/mesh.hpp>

namespace legion::core
{
    class gltf_faux_image_loader : public assets::AssetLoader<image>
    {
    public:
        using base = assets::AssetLoader<image>;
        using base::asset_ptr;
        using base::import_cfg;
        using base::progress_type;

        virtual bool canLoad(const fs::view& file) override { return false; }

        virtual common::result<asset_ptr> load(id_type nameHash, const fs::view& file, const import_cfg& settings) override { return legion_exception_msg("Tried to use a faux loader"); }

        virtual void free(image& asset) override { delete[] asset.data(); }
    };

    class gltf_mesh_loader : public assets::AssetLoader<mesh>
    {
    public:
        using base = assets::AssetLoader<mesh>;
        using base::asset_ptr;
        using base::import_cfg;
        using base::progress_type;

        virtual bool canLoad(const fs::view& file) override;

        virtual common::result<asset_ptr> load(id_type nameHash, const fs::view& file, const import_cfg& settings) override;
    };
}
