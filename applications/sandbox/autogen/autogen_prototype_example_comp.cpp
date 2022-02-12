#pragma once
#include "autogen_prototype_example_comp.hpp"
#include "../../sandbox\systems\examplesystem.hpp"
namespace legion::core
{
    L_NODISCARD prototype make_prototype(const example_comp& obj)
    {
        prototype prot;
        prot.typeId = typeHash<example_comp>();
        prot.typeName = "example_comp";
        prot.members = std::vector<member_value>{
            member_value
            {
                "value",
                primitive_value {typeHash<int>(),std::make_any<int>(obj.value)}
            }
        }
        ;return prot;
    }
}
