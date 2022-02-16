#include "autogen_prototype_window.hpp"
#include "../../application/window/window.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<legion::application::window>(const legion::application::window& obj)
    {
        prototype prot;
        prot.typeId = typeHash<legion::application::window>();
        prot.typeName = "legion::application::window";
        {
            static const reflectable_attribute reflectable_attr{};
            prot.attributes.push_back(std::cref(reflectable_attr));
        }
        prot.members = std::vector<member_value>();
        return prot;
    }
}
