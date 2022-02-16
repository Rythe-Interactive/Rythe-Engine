#include "autogen_prototype_example_comp.hpp"
#include "../../sandbox\systems\examplesystem.hpp"
namespace legion::core
{
    template<>
    L_NODISCARD prototype make_prototype<legion::core::example_comp>(const legion::core::example_comp& obj)
    {
        prototype prot;
        prot.typeId = typeHash<legion::core::example_comp>();
        prot.typeName = "legion::core::example_comp";
        {
            static const reflectable_attribute reflectable_attr{};
            prot.attributes.push_back(std::cref(reflectable_attr));
        }
        prot.members = std::vector<member_value>{
        member_value
        {
            "value",
            primitive_value {typeHash<int>(),std::make_any<int>(obj.value)}
        }
        };
        {
            static const range_attribute range_attr{};
            prot.members[0].attributes.push_back(std::cref(range_attr));
        }
        return prot;
    }
}
