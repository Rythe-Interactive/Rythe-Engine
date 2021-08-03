#pragma once
#include <unordered_map>
#include <vector>
#include <memory>

#include <core/platform/platform.hpp>
#include <core/filesystem/filesystem.hpp>
#include <core/common/result.hpp>
#include <core/async/async_operation.hpp>

#include <core/assets/asset.hpp>
#include <core/assets/assetloader.hpp>
#include <core/assets/import_settings.hpp>

namespace legion::core::assets
{
    namespace detail
    {
        struct asset_info
        {
            std::string name;
            size_type assetLoader;
        };
    }

    template<typename AssetType>
    class AssetCache
    {
        using asset_ptr = asset<AssetType>;
        using import_cfg = import_settings<AssetType>;
        using loader_type = AssetLoader<AssetType>;

    private:
        static std::unordered_map<id_type, AssetType> m_cache;
        static std::unordered_map<id_type, detail::asset_info> m_info;

        static std::vector<std::unique_ptr<loader_type>> m_loaders;

        static bool hasLoaders() noexcept;

    public:
        static void addLoader(std::unique_ptr<loader_type>&& loader);
        template<typename LoaderType, typename... Arguments>
        static void addLoader(Arguments&&... args);

        static common::result<asset_ptr> load(const fs::view& file);
        static common::result<asset_ptr> load(const fs::view& file, const import_cfg& settings);
        static common::result<asset_ptr> load(const std::string& name, const fs::view& file);
        static common::result<asset_ptr> load(const std::string& name, const fs::view& file, const import_cfg& settings);
        static common::result<asset_ptr> load(id_type nameHash, const std::string& name, const fs::view& file);
        static common::result<asset_ptr> load(id_type nameHash, const std::string& name, const fs::view& file, const import_cfg& settings);
        static async::async_operation<common::result<asset_ptr>> loadAsync(const fs::view& file);
        static async::async_operation<common::result<asset_ptr>> loadAsync(const fs::view& file, const import_cfg& settings);
        static async::async_operation<common::result<asset_ptr>> loadAsync(const std::string& name, const fs::view& file);
        static async::async_operation<common::result<asset_ptr>> loadAsync(const std::string& name, const fs::view& file, const import_cfg& settings);
        static async::async_operation<common::result<asset_ptr>> loadAsync(id_type nameHash, const std::string& name, const fs::view& file);
        static async::async_operation<common::result<asset_ptr>> loadAsync(id_type nameHash, const std::string& name, const fs::view& file, const import_cfg& settings);

        static asset_ptr create(const std::string& name);
        static asset_ptr create(const std::string& name, const AssetType& src);
        template<typename... Arguments>
        static asset_ptr create(const std::string& name, Arguments&&... args);
        static asset_ptr create(id_type nameHash, const std::string& name);
        static asset_ptr create(id_type nameHash, const std::string& name, const AssetType& src);
        template<typename... Arguments>
        static asset_ptr create(id_type nameHash, const std::string& name, Arguments&&... args);

        static asset_ptr has(const std::string& name);
        static asset_ptr has(id_type nameHash);

        static asset_ptr get(const std::string& name);
        static asset_ptr get(id_type nameHash);

        static void destroy(const std::string& name);
        static void destroy(id_type nameHash);
    };
}

#include <core/assets/assets.inl>
