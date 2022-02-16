#include "autogen_prototype_camera.hpp"
#include "../../rendering/components/camera.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<legion::rendering::camera>(const legion::rendering::camera& obj)
    {
        prototype prot;
        prot.typeId = typeHash<legion::rendering::camera>();
        prot.typeName = "legion::rendering::camera";
        {
            static const reflectable_attribute reflectable_attr{};
            prot.attributes.push_back(std::cref(reflectable_attr));
        }
        prot.members = std::vector<member_value>{
        member_value
        {
            "fov",
            primitive_value {typeHash<float>(),std::make_any<float>(obj.fov)}
        }
        ,
        member_value
        {
            "nearz",
            primitive_value {typeHash<float>(),std::make_any<float>(obj.nearz)}
        }
        ,
        member_value
        {
            "farz",
            primitive_value {typeHash<float>(),std::make_any<float>(obj.farz)}
        }
        };
        {
            auto nested_prot = make_prototype(obj.targetWindow);
            prot.members.emplace_back("targetWindow",nested_prot);
        }
        {
            auto nested_prot = make_prototype(obj.renderTarget);
            prot.members.emplace_back("renderTarget",nested_prot);
        }
        {
            auto nested_prot = make_prototype(obj.clearColor);
            prot.members.emplace_back("clearColor",nested_prot);
        }
        return prot;
    }
}
