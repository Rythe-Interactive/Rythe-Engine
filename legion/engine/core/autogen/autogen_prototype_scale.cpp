#pragma once
#include "autogen_prototype_scale.hpp"
#include "../../core\defaults\defaultcomponents.hpp"
namespace legion::core
{
    L_NODISCARD prototype make_prototype(const scale& obj)
    {
        prototype prot;
        prot.typeId = typeHash<scale>();
        prot.typeName = "scale";
        prot.members = std::vector<member_value>();
        return prot;
    }
}
