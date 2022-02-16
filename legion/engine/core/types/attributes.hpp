#pragma once
#include <core/types/type_util.hpp>
#include <core/types/primitives.hpp>

namespace legion::core
{
    struct attribute_base 
    {
        virtual std::string_view get_name() LEGION_PURE;
        virtual id_type get_id() LEGION_PURE;
    };

    template<typename attrib>
    struct attribute : public attribute_base
    {
        virtual std::string_view get_name() override { return nameOfType<attrib>(); }
        virtual id_type get_id() override { return typeHash<attrib>(); }
    };

    struct reflectable_attribute : public attribute<reflectable_attribute>
    {

    };

    struct range_attribute : public attribute<range_attribute>
    {

    };
}
