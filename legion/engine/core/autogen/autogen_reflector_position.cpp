#pragma once
#include "autogen_reflector_position.hpp"
#include "../../core\defaults\defaultcomponents.hpp"
namespace legion::core
{
    L_NODISCARD reflector make_reflector(position& obj)
    {
        reflector refl;
        refl.typeId = typeHash<position>();
        refl.typeName = "position";
        refl.members = std::vector<member_reference>();
        refl.data = std::addressof(obj);
        return refl;
    }
    L_NODISCARD const reflector make_reflector(const position& obj)
    {
        ptr_type address = reinterpret_cast<ptr_type>(std::addressof(obj));
        reflector refl;
        refl.typeId = typeHash<position>();
        refl.typeName = "position";
        refl.members = std::vector<member_reference>();
        refl.data = reinterpret_cast<void*>(address);
        return refl;
    }
}
