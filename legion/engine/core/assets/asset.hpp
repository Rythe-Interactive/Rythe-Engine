#pragma once
#include <core/containers/pointer.hpp>

namespace legion::core::assets
{
    template<typename AssetType>
    struct asset : public pointer<AssetType>
    {
        template<typename T>
        friend class AssetCache;
    private:
        id_type m_id;

    public:
        id_type id() const noexcept;
        const std::string& name() const;
        void destroy();
        asset copy(const std::string& name) const;
        asset copy(id_type nameHash) const;
    };
}
