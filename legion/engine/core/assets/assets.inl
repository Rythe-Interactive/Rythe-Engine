#include <core/assets/assets.hpp>
#pragma once
#include <core/scheduling/scheduling.hpp>
#include <core/async/async.hpp>
#include "assetcache.hpp"

namespace legion::core::assets
{
    template<typename AssetType>
    inline L_ALWAYS_INLINE bool legion::core::assets::AssetCache<AssetType>::hasLoaders() noexcept
    {
        return !m_loaders.empty();;
    }

    template<typename AssetType>
    template<typename... Args>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::createInternal(id_type nameHash, const std::string& name, id_type loaderId, Args && ...args)
    {
        static_assert(std::is_constructible_v<AssetType, Args...>, "Asset type is not constructible with given argument types.");

        AssetType* ptr = &(*m_cache.try_emplace(nameHash, AssetType(std::forward<Args>(args)...)).first);
        m_info.try_emplace(nameHash, { name, loaderId });
        return asset<AssetType>{ {ptr}, nameHash };
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE void AssetCache<AssetType>::addLoader(std::unique_ptr<AssetLoader<AssetType>>&& loader)
    {
        loader->m_loaderId = m_loaders.size() + 1;
        m_loaders.push_back(std::move(loader));
    }

    template<typename AssetType>
    template<typename LoaderType, typename... Arguments>
    inline void AssetCache<AssetType>::addLoader(Arguments&& ...args)
    {
        static_assert(std::is_base_of_v<AssetLoader<AssetType>, LoaderType>, "Loader must be a loader of the same asset type.");

        auto* ptr = new LoaderType(std::forward<Arguments>(args)...);
        ptr->m_loaderId = m_loaders.size() + 1;
        m_loaders.emplace_back(ptr);
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
    inline common::result<asset<AssetType>> AssetCache<AssetType>::load(const fs::view& file, const AssetCache<AssetType>::import_cfg& settings)
    {
        auto result = file.get_filename();
        if (!result)
            return { legion_exception_msg(result.error().what()), result.warnings() };
        return load(nameHash(name), result.value(), file, settings);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE common::result<asset<AssetType>> AssetCache<AssetType>::load(const std::string& name, const fs::view& file)
    {
        return load(nameHash(name), name, file, import_settings<AssetType>{});
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE common::result<asset<AssetType>> AssetCache<AssetType>::load(const std::string& name, const fs::view& file, const AssetCache<AssetType>::import_cfg& settings)
    {
        return load(nameHash(name), name, file, settings);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE common::result<asset<AssetType>> legion::core::assets::AssetCache<AssetType>::load(id_type nameHash, const std::string& name, const fs::view& file)
    {
        return load(nameHash, name, file, import_settings<AssetType>{});
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::load(id_type nameHash, const std::string& name, const fs::view& file, const AssetCache<AssetType>::import_cfg& settings)
    {
        loader_type* loader;
        size_type loaderId;

        for (size_type i = m_loaders.size() - 1; i >= 0; i--)
            if (m_loaders[i]->canLoad(file))
            {
                loader = m_loaders[i].get();
                loaderId = i + 1;
                break;
            }

        return loader->load(nameHash, name, file, settings);
    }

    template<typename AssetType>
    inline async::async_operation<common::result<asset<AssetType>>> AssetCache<AssetType>::loadAsync(const fs::view& file)
    {
        auto result = file.get_filename();
        if (!result)
        {
            auto progress = std::make_shared<async::async_progress<common::result<asset<AssetType>>>>(0.f);

            progress->complete(legion_exception_msg(result.error().what()), result.warnings());

            return async::async_operation{ progress };
        }

        return loadAsync(nameHash(name), result.value(), file, import_settings<AssetType>{});
    }

    template<typename AssetType>
    inline async::async_operation<common::result<asset<AssetType>>> AssetCache<AssetType>::loadAsync(const fs::view& file, const AssetCache<AssetType>::import_cfg& settings)
    {
        auto result = file.get_filename();
        if (!result)
        {
            auto progress = std::make_shared<async::async_progress<common::result<asset<AssetType>>>>(0.f);

            progress->complete(legion_exception_msg(result.error().what()), result.warnings());

            return async::async_operation{ progress };
        }

        return loadAsync(nameHash(name), result.value(), file, settings);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE async::async_operation<common::result<asset<AssetType>>> AssetCache<AssetType>::loadAsync(const std::string& name, const fs::view& file)
    {
        return loadAsync(nameHash(name), name, file, import_settings<AssetType>{});
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE async::async_operation<common::result<asset<AssetType>>> AssetCache<AssetType>::loadAsync(const std::string& name, const fs::view& file, const AssetCache<AssetType>::import_cfg& settings)
    {
        return loadAsync(nameHash(name), name, file, settings);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE async::async_operation<common::result<asset<AssetType>>> AssetCache<AssetType>::loadAsync(id_type nameHash, const std::string& name, const fs::view& file)
    {
        return loadAsync(nameHash, name, file, import_settings<AssetType>{});
    }

    template<typename AssetType>
    inline async::async_operation<common::result<asset<AssetType>>> AssetCache<AssetType>::loadAsync(id_type nameHash, const std::string& name, const fs::view& file, const AssetCache<AssetType>::import_cfg& settings)
    {
        auto progress = std::make_shared<async::async_progress<common::result<asset<AssetType>>>>(0.f);

        progress->complete(load(nameHash, name, file, settings);

        return async::async_operation{ progress }; // Requires ability to schedule a single task. or job system not to halt until all jobs are done.
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> AssetCache<AssetType>::create(const std::string& name)
    {
        return create(nameHash(name), name);
    }

    template<typename AssetType>
    inline asset<AssetType> AssetCache<AssetType>::create(id_type nameHash, const std::string& name)
    {
        static_assert(std::is_default_constructible_v<AssetType>, "Asset type is not default constructible.");

        AssetType* ptr = &(m_cache.[nameHash]); // Slightly faster than try_emplace in most cases except for when using libstdc++(GNU) with Clang, with GCC or using libc++(LLVM) with Clang is no issue.
        m_info.try_emplace(nameHash, { name, invalid_id });
        return { {ptr}, nameHash };
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> AssetCache<AssetType>::create(const std::string& name, const AssetType& src)
    {
        return create(nameHash(name), name, src);
    }

    template<typename AssetType>
    inline asset<AssetType>  AssetCache<AssetType>::create(id_type nameHash, const std::string& name, const AssetType& src)
    {
        static_assert(std::is_copy_constructible_v<AssetType>, "Asset type is not copy constructible.");

        AssetType* ptr = &(*m_cache.try_emplace(nameHash, src).first);
        m_info.try_emplace(nameHash, { name, invalid_id });
        return asset<AssetType>{ {ptr}, nameHash };
    }

    template<typename AssetType>
    inline bool AssetCache<AssetType>::has(const std::string& name)
    {
        return m_cache.find(nameHash(name)) != m_cache.end();
    }

    template<typename AssetType>
    inline bool AssetCache<AssetType>::has(id_type nameHash)
    {
        return m_cache.find(nameHash) != m_cache.end();
    }

    template<typename AssetType>
    inline asset<AssetType> AssetCache<AssetType>::get(const std::string& name)
    {
        id_type id = nameHash(name);
        return { {&m_cache.at(id)}, id };
    }

    template<typename AssetType>
    inline asset<AssetType> AssetCache<AssetType>::get(id_type nameHash)
    {
        return { {&m_cache.at(nameHash)}, nameHash };

    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::destroy(const std::string& name)
    {
        id_type id = nameHash(name);

        auto& info = m_info.at(id);
        if (info.assetLoader)
            m_loaders[info.assetLoader - 1]->free(m_cache.at(id));

        m_cache.erase(id);
        m_info.erase(id);
    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::destroy(id_type nameHash)
    {
        auto& info = m_info.at(nameHash);
        if (info.assetLoader)
            m_loaders[info.assetLoader - 1]->free(m_cache.at(nameHash));

        m_cache.erase(nameHash);
        m_info.erase(nameHash);
    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::destroy(asset_ptr asset)
    {
        auto& info = m_info.at(asset.m_id);
        if (info.assetLoader)
            m_loaders[info.assetLoader - 1]->free(*asset.ptr);

        m_cache.erase(asset.m_id);
        m_info.erase(asset.m_id);
    }

    template<typename AssetType>
    template<typename... Arguments>
    inline L_ALWAYS_INLINE asset<AssetType> AssetCache<AssetType>::create(const std::string& name, Arguments&&... args)
    {
        return create(nameHash(name), name, std::forward<Arguments>(args)...);
    }

    template<typename AssetType>
    template<typename... Arguments>
    inline asset<AssetType> AssetCache<AssetType>::create(id_type nameHash, const std::string& name, Arguments&&... args)
    {
        static_assert(std::is_constructible_v<AssetType, Arguments...>, "Asset type is not constructible with given argument types.");

        AssetType* ptr = &(*m_cache.try_emplace(nameHash, AssetType(std::forward<Arguments>(args)...)).first);
        m_info.try_emplace(nameHash, { name, invalid_id });
        return asset<AssetType>{ {ptr}, nameHash };
    }

    template<typename AssetType>
    template<typename... Args>
    inline L_ALWAYS_INLINE asset<AssetType> AssetLoader<AssetType>::create(id_type nameHash, const std::string& name, Args&&...args) const
    {
        return AssetCache<AssetType>::createInternal(nameHash, name, m_loaderId, std::forward<Args>(args)...);
    }
}
