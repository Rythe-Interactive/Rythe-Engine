#include "autogen_reflector_rotation.hpp"
#include "../../core/defaults/defaultcomponents.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD reflector make_reflector<legion::core::rotation>(legion::core::rotation& obj)
    {
        reflector refl;
        refl.typeId = typeHash<legion::core::rotation>();
        refl.typeName = "legion::core::rotation";
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
    L_NODISCARD const reflector make_reflector<const legion::core::rotation>(const legion::core::rotation& obj)
    {
        ptr_type address = reinterpret_cast<ptr_type>(std::addressof(obj));
        reflector refl;
        refl.typeId = typeHash<legion::core::rotation>();
        refl.typeName = "legion::core::rotation";
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
