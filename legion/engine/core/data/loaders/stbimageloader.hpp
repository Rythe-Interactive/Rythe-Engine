#pragma once
#include <core/assets/assets.hpp>
#include <core/data/image.hpp>

namespace legion::core
{
    class StbImageLoader : public assets::AssetLoader<image>
    {
    public:
        using base = assets::AssetLoader<image>;
        using base::asset_ptr;
        using base::import_cfg;
        using base::progress_type;

        virtual bool canLoad(const fs::view& file) override;

        virtual common::result<asset_ptr> load(id_type nameHash, const fs::view& file, const import_cfg& settings) override;

        virtual void free(image& asset) override;
    };
}
