#pragma once
#include <core/containers/pointer.hpp>

namespace legion::core::assets
{
    template<typename AssetType>
    struct asset : public pointer<AssetType>
    {
        template<typename T>
        friend class AssetCache;

        constexpr asset(AssetType* p, id_type id) noexcept : pointer<AssetType>({ p }), m_id(id) {}

        constexpr bool operator==(const asset& other) const noexcept { return m_id == other.m_id && this->ptr == other.ptr; }
        constexpr bool operator!=(const asset& other) const noexcept { return !operator==(other); }

        RULE_OF_5_NOEXCEPT(asset);

    private:
        id_type m_id;

    public:
        id_type id() const noexcept;
        const std::string& name() const;
        const std::string& path() const;
        void destroy();
        asset copy(const std::string& name) const;
        asset copy(id_type nameHash, const std::string& name) const;
    };

    ManualReflector(asset);

    template<typename AssetType>
    constexpr static asset<AssetType> invalid_asset = { nullptr, invalid_id };
}
