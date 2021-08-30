#include <core/assets/assets.hpp>
#include "assetcache.hpp"
#pragma once

namespace legion::core::assets
{
    template<typename AssetType>
    typename AssetCache<AssetType>::data AssetCache<AssetType>::m_data;

    template<typename AssetType>
    inline AssetCache<AssetType>::data::~data()
    {
        std::vector<id_type> toDestroy;
        toDestroy.reserve(AssetCache<AssetType>::m_data.m_cache.size());

        for (auto& [key, value] : AssetCache<AssetType>::m_data.m_cache)
        {
            toDestroy.push_back(key);
        }

        for (auto& id : toDestroy)
        {
            AssetCache<AssetType>::destroy(id);
        }
    }

    template<typename AssetType>
    template<typename... Args>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::createInternal(id_type nameHash, Args && ...args)
    {
        static_assert(std::is_constructible_v<AssetType, Args...>, "Asset type is not constructible with given argument types.");

        AssetType* ptr = &m_data.m_cache.try_emplace(nameHash, AssetType(std::forward<Args>(args)...)).first->second;
        return asset<AssetType>{ ptr, nameHash };
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE const detail::asset_info& AssetCache<AssetType>::info(id_type nameHash)
    {
        return m_data.m_info.at(nameHash);
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::retry_load(const common::result<asset<AssetType>>& previousAttempt, id_type previousLoader, id_type nameHash, const std::string& name, const fs::view& file, const AssetCache<AssetType>::import_cfg& settings)
    {
        loader_type* loader = nullptr;
        size_type loaderId;

        for (size_type i = previousLoader - 2; i != static_cast<size_type>(-1); i--)
            if (m_data.m_loaders[i]->canLoad(file))
            {
                loader = m_data.m_loaders[i].get();
                loaderId = i + 1;
                break;
            }

        if (loader)
        {
            auto result = loader->load(nameHash, file, settings);
            if (result)
            {
                m_data.m_info.try_emplace(nameHash, detail::asset_info{ name, file.get_virtual_path(), loaderId });
                return result;
            }
            return retry_load(result, loaderId, nameHash, name, file, settings);
        }

        return legion_exception;
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE bool AssetCache<AssetType>::hasLoaders() noexcept
    {
        return !m_data.m_loaders.empty();
    }

    template<typename AssetType>
    template<typename LoaderType>
    inline bool AssetCache<AssetType>::hasLoader()
    {
        return m_data.m_loaderIds.find(typeHash<LoaderType>()) != m_data.m_loaderIds.end();
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE void AssetCache<AssetType>::addLoader(std::unique_ptr<AssetLoader<AssetType>>&& loader)
    {
        loader->m_loaderId = m_data.m_loaders.size() + 1;
        m_data.m_loaders.push_back(std::move(loader));
    }

    template<typename AssetType>
    template<typename LoaderType, typename... Arguments>
    inline void AssetCache<AssetType>::addLoader(Arguments&& ...args)
    {
        static_assert(std::is_base_of_v<AssetLoader<AssetType>, LoaderType>, "Loader must be a loader of the same asset type.");

        auto* ptr = new LoaderType(std::forward<Arguments>(args)...);

        m_data.m_loaderIds.emplace(typeHash<LoaderType>(), m_data.m_loaders.size() + 1);
        m_data.m_loaders.emplace_back(ptr);
    }

    template<typename AssetType>
    template<typename LoaderType, typename ...Arguments>
    inline asset<AssetType> AssetCache<AssetType>::createAsLoader(id_type nameHash, const std::string& name, const std::string& path, Arguments && ...args)
    {
        static_assert(std::is_constructible_v<AssetType, Arguments...>, "Asset type is not constructible with given argument types.");

        AssetType* ptr = &m_data.m_cache.try_emplace(nameHash, AssetType(std::forward<Arguments>(args)...)).first->second;
        m_data.m_info.try_emplace(nameHash, detail::asset_info{ name, path, m_data.m_loaderIds.at(typeHash<LoaderType>()) });
        return asset<AssetType>{ ptr, nameHash };
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::load(const fs::view& file)
    {
        auto result = file.get_filename();
        if (!result)
            return { legion_exception_msg(result.error().what()), result.warnings() };
        return load(nameHash(result.value()), result.value(), file, import_settings<AssetType>{});
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::load(const fs::view& file, const AssetCache<AssetType>::import_cfg& settings)
    {
        auto result = file.get_filename();
        if (!result)
            return { legion_exception_msg(result.error().what()), result.warnings() };
        return load(nameHash(result.value()), result.value(), file, settings);
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
    inline L_ALWAYS_INLINE common::result<asset<AssetType>> AssetCache<AssetType>::load(id_type nameHash, const std::string& name, const fs::view& file)
    {
        return load(nameHash, name, file, import_settings<AssetType>{});
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::load(id_type nameHash, const std::string& name, const fs::view& file, const AssetCache<AssetType>::import_cfg& settings)
    {
        auto traits = file.file_info();
        if (!traits.is_valid_path)
        {
            return legion_exception_msg("invalid file traits: not a valid path");
        }
        else if (!traits.exists)
        {
            return legion_exception_msg("invalid file traits: file does not exist");
        }
        else if (!traits.can_be_read)
        {
            return legion_exception_msg("invalid file traits: file cannot be read");
        }

        loader_type* loader = nullptr;
        size_type loaderId;

        for (size_type i = m_data.m_loaders.size() - 1; i != static_cast<size_type>(-1); i--)
            if (m_data.m_loaders[i]->canLoad(file))
            {
                loader = m_data.m_loaders[i].get();
                loaderId = i + 1;
                break;
            }

        if (loader)
        {
            auto result = loader->load(nameHash, file, settings);
            if (result)
            {
                m_data.m_info.try_emplace(nameHash, detail::asset_info{ name, file.get_virtual_path(), loaderId });
                return result;
            }

            if (loaderId != 1u)
            {
                auto retry = retry_load(result, loaderId, nameHash, name, file, settings);

                if (retry)
                {
                    result.mark_handled();
                    return retry;
                }
                else
                    retry.mark_handled();
            }
            return result;
        }

        return legion_exception_msg("No loader found that could load file");
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

        return loadAsync(nameHash(result.value()), result.value(), file, import_settings<AssetType>{});
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

        return loadAsync(nameHash(result.value()), result.value(), file, settings);
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

        progress->complete(load(nameHash, name, file, settings));

        return async::async_operation{ progress }; // Requires ability to schedule a single task. or job system not to halt until all jobs are done.
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> AssetCache<AssetType>::create(const std::string& name)
    {
        return create(nameHash(name), name, std::string(""));
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> AssetCache<AssetType>::create(id_type nameHash, const std::string& name)
    {
        return create(nameHash, name, std::string(""));
    }

    template<typename AssetType>
    inline asset<AssetType> AssetCache<AssetType>::create(id_type nameHash, const std::string& name, const std::string& path)
    {
        static_assert(std::is_default_constructible_v<AssetType>, "Asset type is not default constructible.");

        AssetType* ptr = &(m_data.m_cache[nameHash]); // Slightly faster than try_emplace in most cases except for when using libstdc++(GNU) with Clang, with GCC or using libc++(LLVM) with Clang is no issue.
        m_data.m_info.try_emplace(nameHash, { name, path, invalid_id });
        return { ptr, nameHash };
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> AssetCache<AssetType>::create(const std::string& name, const AssetType& src)
    {
        return create(nameHash(name), name, std::string(""), src);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> AssetCache<AssetType>::create(id_type nameHash, const std::string& name, const AssetType& src)
    {
        return create(nameHash, name, std::string(""), src);
    }

    template<typename AssetType>
    inline asset<AssetType> AssetCache<AssetType>::create(id_type nameHash, const std::string& name, const std::string& path, const AssetType& src)
    {
        static_assert(std::is_copy_constructible_v<AssetType>, "Asset type is not copy constructible.");

        AssetType* ptr = &m_data.m_cache.try_emplace(nameHash, src).first->second;
        m_data.m_info.try_emplace(nameHash, { name, path, invalid_id });
        return { ptr, nameHash };
    }

    template<typename AssetType>
    template<typename... Arguments>
    inline L_ALWAYS_INLINE asset<AssetType> AssetCache<AssetType>::create(const std::string& name, Arguments&&... args)
    {
        return create(nameHash(name), name, std::string(""), std::forward<Arguments>(args)...);
    }

    template<typename AssetType>
    template<typename... Arguments>
    inline L_ALWAYS_INLINE asset<AssetType> AssetCache<AssetType>::create(id_type nameHash, const std::string& name, Arguments&&... args)
    {
        return create(nameHash, name, std::string(""), std::forward<Arguments>(args)...);
    }

    template<typename AssetType>
    template<typename ...Arguments>
    inline asset<AssetType> AssetCache<AssetType>::create(id_type nameHash, const std::string& name, const std::string& path, Arguments && ...args)
    {
        static_assert(std::is_constructible_v<AssetType, Arguments...>, "Asset type is not constructible with given argument types.");

        AssetType* ptr = &m_data.m_cache.try_emplace(nameHash, AssetType(std::forward<Arguments>(args)...)).first->second;
        m_data.m_info.try_emplace(nameHash, detail::asset_info{ name, path, invalid_id });
        return { ptr, nameHash };
    }

    template<typename AssetType>
    template<typename... Args>
    inline L_ALWAYS_INLINE asset<AssetType> AssetLoader<AssetType>::create(id_type nameHash, Args&&...args) const
    {
        return AssetCache<AssetType>::createInternal(nameHash, std::forward<Args>(args)...);
    }

    template<typename AssetType>
    inline bool AssetCache<AssetType>::has(const std::string& name)
    {
        return m_data.m_cache.find(nameHash(name)) != m_data.m_cache.end();
    }

    template<typename AssetType>
    inline bool AssetCache<AssetType>::has(id_type nameHash)
    {
        return m_data.m_cache.find(nameHash) != m_data.m_cache.end();
    }

    template<typename AssetType>
    inline asset<AssetType> AssetCache<AssetType>::get(const std::string& name)
    {
        id_type id = nameHash(name);
        if (has(id))
            return { &m_data.m_cache.at(id), id };
        return invalid_asset<AssetType>;
    }

    template<typename AssetType>
    inline asset<AssetType> AssetCache<AssetType>::get(id_type nameHash)
    {
        if (has(nameHash))
            return { &m_data.m_cache.at(nameHash), nameHash };
        return invalid_asset<AssetType>;
    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::destroy(const std::string& name)
    {
        id_type id = nameHash(name);

        auto& info = m_data.m_info.at(id);
        if (info.assetLoader)
            m_data.m_loaders[info.assetLoader - 1]->free(m_data.m_cache.at(id));

        m_data.m_cache.erase(id);
        m_data.m_info.erase(id);
    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::destroy(id_type nameHash)
    {
        auto& info = m_data.m_info.at(nameHash);
        if (info.assetLoader)
            m_data.m_loaders[info.assetLoader - 1]->free(m_data.m_cache.at(nameHash));

        m_data.m_cache.erase(nameHash);
        m_data.m_info.erase(nameHash);
    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::destroy(asset_ptr asset)
    {
        auto& info = m_data.m_info.at(asset.m_data.m_id);
        if (info.assetLoader)
            m_data.m_loaders[info.assetLoader - 1]->free(*asset.ptr);

        m_data.m_cache.erase(asset.m_data.m_id);
        m_data.m_info.erase(asset.m_data.m_id);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE id_type asset<AssetType>::id() const noexcept
    {
        return m_id;
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE const std::string& asset<AssetType>::name() const
    {
        return AssetCache<AssetType>::info(m_id).name;
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE const std::string& asset<AssetType>::path() const
    {
        return AssetCache<AssetType>::info(m_id).path;
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE void asset<AssetType>::destroy()
    {
        AssetCache<AssetType>::destoy(m_id);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> asset<AssetType>::copy(const std::string& name) const
    {
        return AssetCache<AssetType>::create(name, *this->ptr);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> asset<AssetType>::copy(id_type nameHash, const std::string& name) const
    {
        return AssetCache<AssetType>::create(nameHash, name, *this->ptr);
    }

}