#include "autogen_prototype_light.hpp"
#include "../../rendering\components\light.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<light>(const light& obj)
    {
        prototype prot;
        prot.typeId = typeHash<light>();
        prot.typeName = "light";
        prot.members = std::vector<member_value>();
        return prot;
    }
}
