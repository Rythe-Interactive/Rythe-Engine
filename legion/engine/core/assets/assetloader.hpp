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
        template<typename T>
        friend class AssetCache;
    public:
        using asset_ptr = asset<AssetType>;
        using import_cfg = import_settings<AssetType>;
        using progress_type = async::async_progress<common::result<asset_ptr>>;

    protected:
        template<typename... Args>
        asset_ptr create(id_type nameHash, Args&&... args) const;

    public:
        virtual bool canLoad(const fs::view& file) LEGION_PURE;
        virtual common::result<asset_ptr> load(id_type nameHash, const fs::view& file, const import_cfg& settings) LEGION_PURE;
        virtual common::result<asset_ptr> loadAsync(id_type nameHash, const fs::view& file, const import_cfg& settings, L_MAYBEUNUSED progress_type& progress)
        {
            return load(nameHash, file, settings);
        }

        virtual void free(AssetType& asset) LEGION_IMPURE;

        virtual ~AssetLoader() = default;
    };
}
