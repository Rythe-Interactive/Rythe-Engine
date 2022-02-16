#include "autogen_prototype_rotation.hpp"
#include "../../core/defaults/defaultcomponents.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<legion::core::rotation>(const legion::core::rotation& obj)
    {
        prototype prot;
        prot.typeId = typeHash<legion::core::rotation>();
        prot.typeName = "legion::core::rotation";
        {
            static const reflectable_attribute reflectable_attr{};
            prot.attributes.push_back(std::cref(reflectable_attr));
        }
        prot.members = std::vector<member_value>();
        return prot;
    }
}
