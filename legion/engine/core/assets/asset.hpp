#pragma once
#include <core/containers/pointer.hpp>

namespace legion::core::assets
{
    template<typename AssetType>
    struct asset : public pointer<AssetType>
    {
    private:
        id_type m_id;

    public:
        id_type id();
        void destroy();
        asset copy(const std::string& name);
        asset copy(id_type nameHash);
    };
}
