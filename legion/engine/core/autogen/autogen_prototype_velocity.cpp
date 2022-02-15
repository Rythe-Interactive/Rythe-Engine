#include "autogen_prototype_velocity.hpp"
#include "../../core\defaults\defaultcomponents.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<velocity>(const velocity& obj)
    {
        prototype prot;
        prot.typeId = typeHash<velocity>();
        prot.typeName = "velocity";
        prot.members = std::vector<member_value>();
        return prot;
    }
}
