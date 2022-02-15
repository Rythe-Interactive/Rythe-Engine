#include "autogen_prototype_rotation.hpp"
#include "../../core\defaults\defaultcomponents.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<rotation>(const rotation& obj)
    {
        prototype prot;
        prot.typeId = typeHash<rotation>();
        prot.typeName = "rotation";
        prot.members = std::vector<member_value>();
        return prot;
    }
}
