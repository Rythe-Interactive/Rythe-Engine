#include <core/assets/assets.hpp>
#include <core/scheduling/scheduling.hpp>
#pragma once

namespace legion::core::assets
{
    template<typename AssetType>
    template<typename... Args>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::createInternal(id_type nameHash, Args && ...args)
    {
        static_assert(std::is_constructible_v<AssetType, Args...>, "Asset type is not constructible with given argument types.");

        AssetType* ptr = &instance.m_cache.try_emplace(nameHash, AssetType(std::forward<Args>(args)...)).first->second;
        return asset<AssetType>{ ptr, nameHash };
    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::onShutdown()
    {
        std::vector<id_type> toDestroy;
        toDestroy.reserve(instance.m_cache.size());

        for (auto& [key, value] : instance.m_cache)
            toDestroy.push_back(key);

        for (auto& id : toDestroy)
            destroy(id);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE const detail::asset_info& AssetCache<AssetType>::info(id_type nameHash)
    {
        return instance.m_info.at(nameHash);
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::retryLoad(id_type previousLoader, id_type nameHash, const std::string& name, const fs::view& file, const AssetCache<AssetType>::import_cfg& settings)
    {
        loader_type* loader = nullptr;
        size_type loaderId;

        for (size_type i = previousLoader - 2; i != static_cast<size_type>(-1); i--)
            if (instance.m_loaders[i]->canLoad(file))
            {
                loader = instance.m_loaders[i].get();
                loaderId = i + 1;
                break;
            }

        if (loader)
        {
            auto result = loader->load(nameHash, file, settings);
            if (result)
            {
                instance.m_info.try_emplace(nameHash, detail::asset_info{ name, file.get_virtual_path(), loaderId });
                return result;
            }
            result.mark_handled();
            return retryLoad(loaderId, nameHash, name, file, settings);
        }

        return legion_exception;
    }

    template<typename AssetType>
    inline common::result<asset<AssetType>> AssetCache<AssetType>::retryLoadAsync(id_type previousLoader, id_type nameHash, const std::string& name, const fs::view& file, const import_cfg& settings, AssetCache<AssetType>::progress_type& progress)
    {
        loader_type* loader = nullptr;
        size_type loaderId;

        for (size_type i = previousLoader - 2; i != static_cast<size_type>(-1); i--)
            if (instance.m_loaders[i]->canLoad(file))
            {
                loader = instance.m_loaders[i].get();
                loaderId = i + 1;
                break;
            }

        if (loader)
        {
            progress.reset();
            auto result = loader->loadAsync(nameHash, file, settings, progress);
            if (result)
            {
                instance.m_info.try_emplace(nameHash, detail::asset_info{ name, file.get_virtual_path(), loaderId });
                return result;
            }
            result.mark_handled();
            return retryLoadAsync(loaderId, nameHash, name, file, settings, progress);
        }

        return legion_exception;
    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::asyncLoadJob(id_type nameHash, const std::string& name, const fs::view& file, const AssetCache<AssetType>::import_cfg& settings, const std::shared_ptr<typename AssetCache<AssetType>::progress_type>& progress)
    {
        loader_type* loader = nullptr;
        size_type loaderId;

        for (size_type i = instance.m_loaders.size() - 1; i != static_cast<size_type>(-1); i--)
            if (instance.m_loaders[i]->canLoad(file))
            {
                loader = instance.m_loaders[i].get();
                loaderId = i + 1;
                break;
            }

        if (loader)
        {
            auto result = loader->loadAsync(nameHash, file, settings, *progress);
            if (result)
            {
                instance.m_info.try_emplace(nameHash, detail::asset_info{ name, file.get_virtual_path(), loaderId });
                progress->complete(result);
                return;
            }

            if (loaderId != 1u)
            {
                auto retry = retryLoadAsync(loaderId, nameHash, name, file, settings, *progress);

                if (retry)
                {
                    result.mark_handled();
                    progress->complete(retry);
                    return;
                }
                else
                    retry.mark_handled();
            }
            progress->complete(result);
            return;
        }

        progress->complete(legion_exception_msg("No loader found that could load file"));
        return;
    }

    template<typename AssetType>
    inline byte AssetCache<AssetType>::reportAssetToEngine()
    {
        Engine::subscribeToInit(&AssetCache<AssetType>::init);
        Engine::subscribeToShutdown(&AssetCache<AssetType>::shutdown);
        return 0;
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE bool AssetCache<AssetType>::hasLoaders() noexcept
    {
        return !instance.m_loaders.empty();
    }

    template<typename AssetType>
    template<typename LoaderType>
    inline bool AssetCache<AssetType>::hasLoader()
    {
        return instance.m_loaderIds.find(typeHash<LoaderType>()) != instance.m_loaderIds.end();
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE void AssetCache<AssetType>::addLoader(std::unique_ptr<AssetLoader<AssetType>>&& loader)
    {
        if (!base::initialized())
            throw legion_exception_msg("Asset was not marked as asset, and cache is thus not yet initialized.");

        loader->m_loaderId = instance.m_loaders.size() + 1;
        instance.m_loaders.push_back(std::move(loader));
    }

    template<typename AssetType>
    template<typename LoaderType, typename... Arguments>
    inline void AssetCache<AssetType>::addLoader(Arguments&& ...args)
    {
        static_assert(std::is_base_of_v<AssetLoader<AssetType>, LoaderType>, "Loader must be a loader of the same asset type.");

        if (!base::initialized())
            throw legion_exception_msg("Asset was not marked as asset, and cache is thus not yet initialized.");

        auto* ptr = new LoaderType(std::forward<Arguments>(args)...);

        instance.m_loaderIds.emplace(typeHash<LoaderType>(), instance.m_loaders.size() + 1);
        instance.m_loaders.emplace_back(ptr);
    }

    template<typename AssetType>
    template<typename LoaderType, typename ...Arguments>
    inline asset<AssetType> AssetCache<AssetType>::createAsLoader(id_type nameHash, const std::string& name, const std::string& path, Arguments && ...args)
    {
        static_assert(std::is_constructible_v<AssetType, Arguments...>, "Asset type is not constructible with given argument types.");

        AssetType* ptr = &instance.m_cache.try_emplace(nameHash, AssetType(std::forward<Arguments>(args)...)).first->second;
        instance.m_info.try_emplace(nameHash, detail::asset_info{ name, path, instance.m_loaderIds.at(typeHash<LoaderType>()) });
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

        for (size_type i = instance.m_loaders.size() - 1; i != static_cast<size_type>(-1); i--)
            if (instance.m_loaders[i]->canLoad(file))
            {
                loader = instance.m_loaders[i].get();
                loaderId = i + 1;
                break;
            }

        if (loader)
        {
            auto result = loader->load(nameHash, file, settings);
            if (result)
            {
                instance.m_info.try_emplace(nameHash, detail::asset_info{ name, file.get_virtual_path(), loaderId });
                return result;
            }

            if (loaderId != 1u)
            {
                auto retry = retryLoad(loaderId, nameHash, name, file, settings);

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
        auto progress = std::make_shared<async::async_progress<common::result<asset<AssetType>>>>();

        auto traits = file.file_info();
        if (!traits.is_valid_path)
        {
            progress->complete(legion_exception_msg("invalid file traits: not a valid path"));
            return;
        }
        else if (!traits.exists)
        {
            progress->complete(legion_exception_msg("invalid file traits: file does not exist"));
            return;
        }
        else if (!traits.can_be_read)
        {
            progress->complete(legion_exception_msg("invalid file traits: file cannot be read"));
            return;
        }

        schd::Scheduler::queueJobs(1, [
            nameHash = nameHash,
                name = name,
                file = file,
                settings = settings,
                progress = progress
        ]()
            {
                asyncLoadJob(nameHash, name, file, settings, progress);
            });

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

        AssetType* ptr = &(instance.m_cache[nameHash]); // Slightly faster than try_emplace in most cases except for when using libstdc++(GNU) with Clang, with GCC or using libc++(LLVM) with Clang is no issue.
        instance.m_info.try_emplace(nameHash, { name, path, invalid_id });
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

        AssetType* ptr = &instance.m_cache.try_emplace(nameHash, src).first->second;
        instance.m_info.try_emplace(nameHash, { name, path, invalid_id });
        return { ptr, nameHash };
    }

    template<typename AssetType>
    template<typename... Arguments>
    inline L_ALWAYS_INLINE asset<AssetType> AssetCache<AssetType>::create(const std::string& name, Arguments&&... args)
    {
        return createWithPath(nameHash(name), name, std::string(""), std::forward<Arguments>(args)...);
    }

    template<typename AssetType>
    template<typename... Arguments>
    inline L_ALWAYS_INLINE asset<AssetType> AssetCache<AssetType>::create(id_type nameHash, const std::string& name, Arguments&&... args)
    {
        return createWithPath(nameHash, name, std::string(""), std::forward<Arguments>(args)...);
    }

    template<typename AssetType>
    template<typename ...Arguments>
    inline asset<AssetType> AssetCache<AssetType>::createWithPath(id_type nameHash, const std::string& name, const std::string& path, Arguments && ...args)
    {
        static_assert(std::is_constructible_v<AssetType, Arguments...>, "Asset type is not constructible with given argument types.");

        AssetType* ptr = &instance.m_cache.try_emplace(nameHash, AssetType(std::forward<Arguments>(args)...)).first->second;
        instance.m_info.try_emplace(nameHash, detail::asset_info{ name, path, invalid_id });
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
        return instance.m_cache.find(nameHash(name)) != instance.m_cache.end();
    }

    template<typename AssetType>
    inline bool AssetCache<AssetType>::has(id_type nameHash)
    {
        return instance.m_cache.find(nameHash) != instance.m_cache.end();
    }

    template<typename AssetType>
    inline asset<AssetType> AssetCache<AssetType>::get(const std::string& name)
    {
        id_type id = nameHash(name);
        if (has(id))
            return { &instance.m_cache.at(id), id };
        return invalid_asset<AssetType>;
    }

    template<typename AssetType>
    inline asset<AssetType> AssetCache<AssetType>::get(id_type nameHash)
    {
        if (has(nameHash))
            return { &instance.m_cache.at(nameHash), nameHash };
        return invalid_asset<AssetType>;
    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::destroy(const std::string& name)
    {
        id_type id = nameHash(name);

        auto& m_info = instance.m_info.at(id);
        if (m_info.assetLoader)
            instance.m_loaders[m_info.assetLoader - 1]->free(instance.m_cache.at(id));

        instance.m_cache.erase(id);
        instance.m_info.erase(id);
    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::destroy(id_type nameHash)
    {
        auto& m_info = instance.m_info.at(nameHash);
        if (m_info.assetLoader)
            instance.m_loaders[m_info.assetLoader - 1]->free(instance.m_cache.at(nameHash));

        instance.m_cache.erase(nameHash);
        instance.m_info.erase(nameHash);
    }

    template<typename AssetType>
    inline void AssetCache<AssetType>::destroy(asset_ptr asset)
    {
        auto& m_info = instance.m_info.at(asset.instance.id);
        if (m_info.assetLoader)
            instance.m_loaders[m_info.assetLoader - 1]->free(*asset.ptr);

        instance.m_cache.erase(asset.instance.id);
        instance.m_info.erase(asset.instance.id);
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
