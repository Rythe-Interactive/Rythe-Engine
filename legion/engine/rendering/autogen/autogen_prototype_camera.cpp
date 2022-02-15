#include "autogen_prototype_camera.hpp"
#include "../../rendering\components\camera.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<camera>(const camera& obj)
    {
        prototype prot;
        prot.typeId = typeHash<camera>();
        prot.typeName = "camera";
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
        }
        ;{
            auto nested_prot = make_prototype(obj.targetWindow);
            members.emplace_back(nested_prot);
        }
        {
            auto nested_prot = make_prototype(obj.renderTarget);
            members.emplace_back(nested_prot);
        }
        {
            auto nested_prot = make_prototype(obj.clearColor);
            members.emplace_back(nested_prot);
        }
        return prot;
    }
}
