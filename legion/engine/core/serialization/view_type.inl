#include <core/serialization/view_type.hpp>
#pragma once

namespace legion::core::serialization
{
    using json = nlohmann::json;

    template<typename type>
    static inline json serialization::json_view::serialize(type t)
    {
        prototype<type> temp = prototype<type>(t);
        json j;
        j["Type ID"] = type_hash<type>().local();
        return j;
    }

    template<typename type>
    static inline std::unique_ptr<prototype<type>> serialization::json_view::deserialize(fs::view filePath)
    {
        std::unique_ptr<prototype<type>> t;
        return t;
    }

    template<typename type>
    inline void serialization::json_view::store(type t)
    {

    }

    template<typename type>
    inline type serialization::json_view::load()
    {
        type t;
        return t;
    }

}
