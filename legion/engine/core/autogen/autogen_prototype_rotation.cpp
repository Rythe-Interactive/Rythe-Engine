#pragma once
#include "autogen_prototype_rotation.hpp"
#include "../../core\defaults\defaultcomponents.hpp"
namespace legion::core
{
    L_NODISCARD prototype make_prototype(const rotation& obj)
    {
        prototype prot;
        prot.typeId = typeHash<rotation>();
        prot.typeName = "rotation";
        prot.members = std::vector<member_value>();
        return prot;
    }
}
