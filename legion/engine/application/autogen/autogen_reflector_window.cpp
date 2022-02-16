#include "autogen_reflector_window.hpp"
#include "../../application/window/window.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD reflector make_reflector<legion::application::window>(legion::application::window& obj)
    {
        reflector refl;
        refl.typeId = typeHash<legion::application::window>();
        refl.typeName = "legion::application::window";
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
    L_NODISCARD const reflector make_reflector<const legion::application::window>(const legion::application::window& obj)
    {
        ptr_type address = reinterpret_cast<ptr_type>(std::addressof(obj));
        reflector refl;
        refl.typeId = typeHash<legion::application::window>();
        refl.typeName = "legion::application::window";
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
