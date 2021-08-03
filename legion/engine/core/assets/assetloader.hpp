#pragma once

#include <core/filesystem/filesystem.hpp>
#include <core/common/result.hpp>
#include <core/async/async_operation.hpp>

#include <core/assets/asset.hpp>
#include <core/assets/import_settings.hpp>

namespace legion::core::assets
{
    template<typename AssetType>
    class AssetLoader
    {
        using asset_ptr = asset<AssetType>;
        using import_cfg = import_settings<AssetType>;
        using progress_type = async::async_progress<common::result<asset_ptr>>;

        virtual bool canLoad(const fs::view& file) LEGION_PURE;
        virtual common::result<void> load(asset_ptr ptr, const fs::view& file, const import_cfg& settings) LEGION_PURE;
        virtual common::result<void> loadAsync(asset_ptr ptr, const fs::view& file, const import_cfg& settings, progress_type& progress)
        {
            load(ptr, file, settings);
            progress.complete(ptr);
        }

        virtual void free(asset_ptr ptr) LEGION_IMPURE;

        virtual ~AssetLoader() = default;
    };
}
