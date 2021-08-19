#include <core/assets/functionalbinds.hpp>
#pragma once

namespace legion::core::assets
{
    template<typename AssetType>
    inline L_ALWAYS_INLINE common::result<asset<AssetType>> load(const fs::view& file)
    {
        return AssetCache<AssetType>::load(file);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE common::result<asset<AssetType>> load(const fs::view& file, const import_settings<AssetType>& settings)
    {
        return AssetCache<AssetType>::load(file, settings);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE common::result<asset<AssetType>> load(const std::string& name, const fs::view& file)
    {
        return AssetCache<AssetType>::load(name, file);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE common::result<asset<AssetType>> load(const std::string& name, const fs::view& file, const import_settings<AssetType>& settings)
    {
        return AssetCache<AssetType>::load(name, file, settings);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE common::result<asset<AssetType>> load(id_type nameHash, const std::string& name, const fs::view& file)
    {
        return AssetCache<AssetType>::load(nameHash, name, file);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE common::result<asset<AssetType>> load(id_type nameHash, const std::string& name, const fs::view& file, const import_settings<AssetType>& settings)
    {
        return AssetCache<AssetType>::load(nameHash, name, file, settings);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE async::async_operation<common::result<asset<AssetType>>> loadAsync(const fs::view& file)
    {
        return AssetCache<AssetType>::loadAsync(file);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE async::async_operation<common::result<asset<AssetType>>> loadAsync(const fs::view& file, const import_settings<AssetType>& settings)
    {
        return AssetCache<AssetType>::loadAsync(file, settings);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE async::async_operation<common::result<asset<AssetType>>> loadAsync(const std::string& name, const fs::view& file)
    {
        return AssetCache<AssetType>::loadAsync(name, file);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE async::async_operation<common::result<asset<AssetType>>> loadAsync(const std::string& name, const fs::view& file, const import_settings<AssetType>& settings)
    {
        return AssetCache<AssetType>::loadAsync(name, file, settings);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE async::async_operation<common::result<asset<AssetType>>> loadAsync(id_type nameHash, const std::string& name, const fs::view& file)
    {
        return AssetCache<AssetType>::loadAsync(nameHash, name, file);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE async::async_operation<common::result<asset<AssetType>>> loadAsync(id_type nameHash, const std::string& name, const fs::view& file, const import_settings<AssetType>& settings)
    {
        return AssetCache<AssetType>::loadAsync(nameHash, name, file, settings);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> create(const std::string& name)
    {
        return AssetCache<AssetType>::create(name);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<remove_cvr_t<AssetType>> create(const std::string& name, AssetType&& src)
    {
        return AssetCache<remove_cvr_t<AssetType>>::create(name, std::forward<AssetType>(src));
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> create(const std::string& name, const AssetType& src)
    {
        return AssetCache<AssetType>::create(name, src);
    }

    template<typename AssetType, typename... Arguments>
    inline L_ALWAYS_INLINE asset<AssetType> create(const std::string& name, Arguments&&... args)
    {
        return AssetCache<AssetType>::create(name, std::forward<Arguments>(args)...);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> create(id_type nameHash, const std::string& name)
    {
        return AssetCache<AssetType>::create(nameHash, name);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<remove_cvr_t<AssetType>> create(id_type nameHash, const std::string& name, AssetType&& src)
    {
        return AssetCache<remove_cvr_t<AssetType>>::create(nameHash, name, std::forward<AssetType>(src));
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> create(id_type nameHash, const std::string& name, const AssetType& src)
    {
        return AssetCache<AssetType>::create(nameHash, name, src);
    }

    template<typename AssetType, typename... Arguments>
    inline L_ALWAYS_INLINE asset<AssetType> create(id_type nameHash, const std::string& name, Arguments&&... args)
    {
        return AssetCache<AssetType>::create(nameHash, name, std::forward<Arguments>(args)...);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE bool exists(const std::string& name)
    {
        return AssetCache<AssetType>::has(name);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE bool exists(id_type nameHash)
    {
        return AssetCache<AssetType>::has(nameHash);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> get(const std::string& name)
    {
        return AssetCache<AssetType>::get(name);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE asset<AssetType> get(id_type nameHash)
    {
        return AssetCache<AssetType>::get(nameHash);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE void destroy(const std::string& name)
    {
        AssetCache<AssetType>::destroy(name);
    }

    template<typename AssetType>
    inline L_ALWAYS_INLINE void destroy(id_type nameHash)
    {
        AssetCache<AssetType>::destroy(nameHash);
    }

    template<typename AssetType>
    void destroy(asset<AssetType> asset)
    {
        AssetCache<AssetType>::destroy(asset);
    }
}
