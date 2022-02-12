#pragma once
#include "autogen_reflector_scale.hpp"
#include "../../core\defaults\defaultcomponents.hpp"
namespace legion::core
{
    L_NODISCARD reflector make_reflector(scale& obj)
    {
        reflector refl;
        refl.typeId = typeHash<scale>();
        refl.typeName = "scale";
        refl.members = std::vector<member_reference>();
        refl.data = std::addressof(obj);
        return refl;
    }
    L_NODISCARD const reflector make_reflector(const scale& obj)
    {
        ptr_type address = reinterpret_cast<ptr_type>(std::addressof(obj));
        reflector refl;
        refl.typeId = typeHash<scale>();
        refl.typeName = "scale";
        refl.members = std::vector<member_reference>();
        refl.data = reinterpret_cast<void*>(address);
        return refl;
    }
}
