#include "autogen_prototype_position.hpp"
#include "../../core\defaults\defaultcomponents.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<position>(const position& obj)
    {
        prototype prot;
        prot.typeId = typeHash<position>();
        prot.typeName = "position";
        prot.members = std::vector<member_value>();
        return prot;
    }
}
