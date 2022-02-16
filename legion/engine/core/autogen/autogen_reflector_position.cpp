#include "autogen_reflector_position.hpp"
#include "../../core/defaults/defaultcomponents.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD reflector make_reflector<legion::core::position>(legion::core::position& obj)
    {
        reflector refl;
        refl.typeId = typeHash<legion::core::position>();
        refl.typeName = "legion::core::position";
        {
            static const reflectable_attribute reflectable_attr{};
            refl.attributes.push_back(std::cref(reflectable_attr));
        }
        refl.members = std::vector<member_reference>
        ();
        refl.data = std::addressof(obj);
        return refl;
    }
    template<>
    L_NODISCARD const reflector make_reflector<const legion::core::position>(const legion::core::position& obj)
    {
        ptr_type address = reinterpret_cast<ptr_type>(std::addressof(obj));
        reflector refl;
        refl.typeId = typeHash<legion::core::position>();
        refl.typeName = "legion::core::position";
        {
            static const reflectable_attribute reflectable_attr{};
            refl.attributes.push_back(std::cref(reflectable_attr));
        }
        refl.members = std::vector<member_reference>
        ();
        refl.data = reinterpret_cast<void*>(address);
        return refl;
    }
}
