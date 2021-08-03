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
        id_type id();
        const std::string& name();
        void destroy();
        asset copy(const std::string& name);
        asset copy(id_type nameHash);
    };
}
