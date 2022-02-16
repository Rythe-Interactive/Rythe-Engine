#include "autogen_prototype_scale.hpp"
#include "../../core/defaults/defaultcomponents.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<legion::core::scale>(const legion::core::scale& obj)
    {
        prototype prot;
        prot.typeId = typeHash<legion::core::scale>();
        prot.typeName = "legion::core::scale";
        {
            static const reflectable_attribute reflectable_attr{};
            prot.attributes.push_back(std::cref(reflectable_attr));
        }
        prot.members = std::vector<member_value>();
        return prot;
    }
}
