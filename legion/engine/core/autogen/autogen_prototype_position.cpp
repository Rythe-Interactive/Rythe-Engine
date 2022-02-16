#include "autogen_prototype_position.hpp"
#include "../../core/defaults/defaultcomponents.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<legion::core::position>(const legion::core::position& obj)
    {
        prototype prot;
        prot.typeId = typeHash<legion::core::position>();
        prot.typeName = "legion::core::position";
        {
            static const reflectable_attribute reflectable_attr{};
            prot.attributes.push_back(std::cref(reflectable_attr));
        }
        prot.members = std::vector<member_value>();
        return prot;
    }
}
