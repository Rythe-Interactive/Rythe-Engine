#include "autogen_reflector_light.hpp"
#include "../../rendering\components\light.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD reflector make_reflector<light>(light& obj)
    {
        reflector refl;
        refl.typeId = typeHash<light>();
        refl.typeName = "light";
        refl.members = std::vector<member_reference>();
        refl.data = std::addressof(obj);
        return refl;
    }
    template<>
    L_NODISCARD const reflector make_reflector<const light>(const light& obj)
    {
        ptr_type address = reinterpret_cast<ptr_type>(std::addressof(obj));
        reflector refl;
        refl.typeId = typeHash<light>();
        refl.typeName = "light";
        refl.members = std::vector<member_reference>();
        refl.data = reinterpret_cast<void*>(address);
        return refl;
    }
}
