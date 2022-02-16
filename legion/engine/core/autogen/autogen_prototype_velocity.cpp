#include "autogen_prototype_velocity.hpp"
#include "../../core/defaults/defaultcomponents.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<legion::core::velocity>(const legion::core::velocity& obj)
    {
        prototype prot;
        prot.typeId = typeHash<legion::core::velocity>();
        prot.typeName = "legion::core::velocity";
        {
            static const reflectable_attribute reflectable_attr{};
            prot.attributes.push_back(std::cref(reflectable_attr));
        }
        prot.members = std::vector<member_value>();
        return prot;
    }
}
