#include "autogen_reflector_camera.hpp"
#include "../../rendering/components/camera.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD reflector make_reflector<legion::rendering::camera>(legion::rendering::camera& obj)
    {
        reflector refl;
        refl.typeId = typeHash<legion::rendering::camera>();
        refl.typeName = "legion::rendering::camera";
        {
            static const reflectable_attribute reflectable_attr{};
            refl.attributes.push_back(std::cref(reflectable_attr));
        }
        refl.members = std::vector<member_reference>
        {
        member_reference
        {
            "fov",
            primitive_reference {typeHash<float>(), &obj.fov}
        }
        ,
        member_reference
        {
            "nearz",
            primitive_reference {typeHash<float>(), &obj.nearz}
        }
        ,
        member_reference
        {
            "farz",
            primitive_reference {typeHash<float>(), &obj.farz}
        }
        };
        {
            auto nested_refl = make_reflector(obj.targetWindow);
            refl.members.emplace_back("targetWindow",nested_refl);
        }
        {
            auto nested_refl = make_reflector(obj.renderTarget);
            refl.members.emplace_back("renderTarget",nested_refl);
        }
        {
            auto nested_refl = make_reflector(obj.clearColor);
            refl.members.emplace_back("clearColor",nested_refl);
        }
        refl.data = std::addressof(obj);
        return refl;
    }
    template<>
    L_NODISCARD const reflector make_reflector<const legion::rendering::camera>(const legion::rendering::camera& obj)
    {
        ptr_type address = reinterpret_cast<ptr_type>(std::addressof(obj));
        reflector refl;
        refl.typeId = typeHash<legion::rendering::camera>();
        refl.typeName = "legion::rendering::camera";
        {
            static const reflectable_attribute reflectable_attr{};
            refl.attributes.push_back(std::cref(reflectable_attr));
        }
        refl.members = std::vector<member_reference>
        {
        member_reference
        {
            "fov",
            primitive_reference {typeHash<float>(), &obj.fov}
        }
        ,
        member_reference
        {
            "nearz",
            primitive_reference {typeHash<float>(), &obj.nearz}
        }
        ,
        member_reference
        {
            "farz",
            primitive_reference {typeHash<float>(), &obj.farz}
        }
        };
        {
            auto nested_refl = make_reflector(obj.targetWindow);
            refl.members.emplace_back("targetWindow",nested_refl);
        }
        {
            auto nested_refl = make_reflector(obj.renderTarget);
            refl.members.emplace_back("renderTarget",nested_refl);
        }
        {
            auto nested_refl = make_reflector(obj.clearColor);
            refl.members.emplace_back("clearColor",nested_refl);
        }
        refl.data = reinterpret_cast<void*>(address);
        return refl;
    }
}
