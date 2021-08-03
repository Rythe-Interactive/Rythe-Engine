#include <core/assets/assetcache.hpp>
#pragma once
#include <core/scheduling/scheduling.hpp>
#include <core/async/async.hpp>

namespace legion::core::assets
{
    template<typename AssetType>
    inline bool legion::core::assets::AssetCache<AssetType>::hasLoaders() noexcept
    {
        return !m_loaders.empty();;
    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::addLoader(std::unique_ptr<AssetLoader<AssetType>>&& loader)
    {
        m_loaders.push_back(std::move(loader));
    }

    template<typename AssetType>
    template<typename LoaderType, typename... Arguments>
    inline void AssetCache<AssetType>::addLoader(Arguments&& ...args)
    {
        static_assert(std::is_base_of_v<AssetLoader<AssetType>, LoaderType>, "Loader must be a loader of the same asset type.");
        m_loaders.emplace_back(new LoaderType(std::forward<Arguments>(args)...));
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::load(const fs::view& file)
    {
        auto result = file.get_filename();
        if (!result)
            return { legion_exception_msg(result.error().what()), result.warnings() };
        return load(nameHash(name), result.value(), file, import_settings<AssetType>{});
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::load(const fs::view& file, const import_settings<AssetType>& settings)
    {
        auto result = file.get_filename();
        if (!result)
            return { legion_exception_msg(result.error().what()), result.warnings() };
        return load(nameHash(name), result.value(), file, settings);
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::load(const std::string& name, const fs::view& file)
    {
        return load(nameHash(name), name, file, import_settings<AssetType>{});
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::load(const std::string& name, const fs::view& file, const import_settings<AssetType>& settings)
    {
        return load(nameHash(name), name, file, settings);
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> legion::core::assets::AssetCache<AssetType>::load(id_type nameHash, const std::string& name, const fs::view& file)
    {
        return load(nameHash, name, file, import_settings<AssetType>{});
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::load(id_type nameHash, const std::string& name, const fs::view& file, const import_settings<AssetType>& settings)
    {
        loader_type* loader;
        size_type loaderId;

        for (size_type i = m_loaders.size() - 1; i >= 0; i--)
            if (m_loaders[i]->canLoad(file))
            {
                loader = m_loaders[i].get();
                loaderId = i;
                break;
            }

        AssetType* ptr = &(*m_cache.try_emplace(nameHash, AssetType{}).first);
        m_info.try_emplace(nameHash, { name, loaderId });
        asset<AssetType> handle{ {ptr}, nameHash };


        auto result = loader->load(handle, file, settings);

        if (result)
            return { handle, result.warnings() };
        return { result.error(), result.warnings() };
    }

    template<typename AssetType>
    inline async::async_operation<common::result<asset<AssetType>>> AssetCache<AssetType>::loadAsync(id_type nameHash, const std::string& name, const fs::view& file, const import_settings<AssetType>& settings)
    {
        return async::async_operation<common::result<asset<AssetType>>>(); // requires ability to schedule a single task. or job system not to halt until all jobs are done.
    }
}
