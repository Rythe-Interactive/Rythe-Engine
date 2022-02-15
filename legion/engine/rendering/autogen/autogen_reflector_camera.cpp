#include "autogen_reflector_camera.hpp"
#include "../../rendering\components\camera.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD reflector make_reflector<camera>(camera& obj)
    {
        reflector refl;
        refl.typeId = typeHash<camera>();
        refl.typeName = "camera";
        refl.members = std::vector<member_reference>{
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
        }
        ;{
            auto nested_refl = make_reflector(obj.targetWindow);
            members.emplace_back(nested_refl);
        }
        {
            auto nested_refl = make_reflector(obj.renderTarget);
            members.emplace_back(nested_refl);
        }
        {
            auto nested_refl = make_reflector(obj.clearColor);
            members.emplace_back(nested_refl);
        }
        refl.data = std::addressof(obj);
        return refl;
    }
    template<>
    L_NODISCARD const reflector make_reflector<const camera>(const camera& obj)
    {
        ptr_type address = reinterpret_cast<ptr_type>(std::addressof(obj));
        reflector refl;
        refl.typeId = typeHash<camera>();
        refl.typeName = "camera";
        refl.members = std::vector<member_reference>{
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
        }
        ;{
            auto nested_refl = make_reflector(obj.targetWindow);
            members.emplace_back(nested_refl);
        }
        {
            auto nested_refl = make_reflector(obj.renderTarget);
            members.emplace_back(nested_refl);
        }
        {
            auto nested_refl = make_reflector(obj.clearColor);
            members.emplace_back(nested_refl);
        }
        refl.data = reinterpret_cast<void*>(address);
        return refl;
    }
}
