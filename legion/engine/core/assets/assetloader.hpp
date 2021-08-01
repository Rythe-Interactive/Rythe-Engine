#pragma once

#include <core/filesystem/filesystem.hpp>

#include <core/assets/asset.hpp>
#include <core/assets/import_settings.hpp>

namespace legion::core::assets
{
    template<typename AssetType>
    class AssetLoader
    {
        using asset_ptr = asset<AssetType>;
        using import_cfg = import_settings<AssetType>;

        virtual bool canLoad(fs::view file) LEGION_PURE;
        virtual bool load(asset_ptr ptr, fs::view file, import_cfg settings) LEGION_PURE;
        virtual void free(asset_ptr ptr) LEGION_IMPURE;

        virtual ~AssetLoader = default;
    };
}
