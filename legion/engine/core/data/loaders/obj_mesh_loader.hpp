#pragma once
#include <core/assets/assets.hpp>
#include <core/data/mesh.hpp>

namespace legion::core
{
    class obj_mesh_loader : public assets::AssetLoader<mesh>
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
