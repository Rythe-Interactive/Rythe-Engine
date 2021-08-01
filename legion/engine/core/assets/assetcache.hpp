#pragma once
#include <unordered_map>
#include <memory>

#include <core/filesystem/filesystem.hpp>

#include <core/assets/asset.hpp>
#include <core/assets/assetloader.hpp>
#include <core/assets/import_settings.hpp>

namespace legion::core::assets
{
    template<typename AssetType>
    class AssetCache
    {
        using asset_ptr = asset<AssetType>;
        using import_cfg = import_settings<AssetType>;
        using loader_type = AssetLoader<AssetType>;
    private:
        static std::unordered_map<id_type, AssetType> m_cache;
        static std::unordered_map<id_type, std::string> m_names;

        static std::unordered_map<id_type, std::unique_ptr<AssetLoader>> m_loaders;

    public:
        static void addLoader(std::unique_ptr<AssetLoader>&& loader);
        template<typename LoaderType, typename... Arguments>
        static void addLoader(Arguments&&... args);

        static asset_ptr load(fs::view file);
        static asset_ptr load(fs::view file, import_cfg settings);
        static asset_ptr load(const std::string& name, fs::view file);
        static asset_ptr load(const std::string& name, fs::view file, import_cfg settings);
        static asset_ptr load(id_type nameHash, fs::view file);
        static asset_ptr load(id_type nameHash, fs::view file, import_cfg settings);

        static asset_ptr create(const std::string& name);
        static asset_ptr create(const std::string& name, const AssetType& src);
        template<typename... Arguments>
        static asset_ptr create(const std::string& name, Arguments&&... args);
        static asset_ptr create(id_type nameHash);
        static asset_ptr create(id_type nameHash, const AssetType& src);
        template<typename... Arguments>
        static asset_ptr create(id_type nameHash, Arguments&&... args);

        static asset_ptr get(const std::string& name);
        static asset_ptr get(id_type nameHash);

        static void destroy(const std::string& name);
        static void destroy(id_type nameHash);
    };
}
