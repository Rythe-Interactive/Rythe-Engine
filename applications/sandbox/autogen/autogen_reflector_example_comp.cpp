#include "autogen_reflector_example_comp.hpp"
#include "../../sandbox\systems\examplesystem.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD reflector make_reflector<legion::core::example_comp>(legion::core::example_comp& obj)
    {
        reflector refl;
        refl.typeId = typeHash<legion::core::example_comp>();
        refl.typeName = "legion::core::example_comp";
        {
            static const reflectable_attribute reflectable_attr{};
            refl.attributes.push_back(std::cref(reflectable_attr));
        }
        refl.members = std::vector<member_reference>
        {
        member_reference
        {
            "value",
            primitive_reference {typeHash<int>(), &obj.value}
        }
        };
        {
            static const range_attribute range_attr{};
            refl.members[0].attributes.push_back(std::cref(range_attr));
        }
        refl.data = std::addressof(obj);
        return refl;
    }
    template<>
    L_NODISCARD const reflector make_reflector<const legion::core::example_comp>(const legion::core::example_comp& obj)
    {
        ptr_type address = reinterpret_cast<ptr_type>(std::addressof(obj));
        reflector refl;
        refl.typeId = typeHash<legion::core::example_comp>();
        refl.typeName = "legion::core::example_comp";
        {
            static const reflectable_attribute reflectable_attr{};
            refl.attributes.push_back(std::cref(reflectable_attr));
        }
        refl.members = std::vector<member_reference>
        {
        member_reference
        {
            "value",
            primitive_reference {typeHash<int>(), &obj.value}
        }
        };
        {
            static const range_attribute range_attr{};
            refl.members[0].attributes.push_back(std::cref(range_attr));
        }
        refl.data = reinterpret_cast<void*>(address);
        return refl;
    }
}
