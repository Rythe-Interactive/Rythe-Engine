#include <core/serialization/serializer.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename component_type>
    inline json serializer<component_type>::serialize(component_type data)
    {
        return json_view<component_prototype<component_type>>().serialize(component_prototype<component_type>(data));
    }

    template<typename component_type>
    inline component_prototype<component_type> serializer<component_type>::deserialize(json j)
    {
        return json_view<component_prototype<component_type>>().deserialize(j);
    }

    template<typename component_type>
    inline void serializer<component_type>::write(fs::view filePath, component_type data)
    {
        std::ofstream os;
        os.open(fs::view_util::get_view_path(filePath,true));
        json j = serialize(data);
        os << j.dump();
        os.close();
    }

    template<typename component_type>
    inline component_prototype<component_type> serializer<component_type>::read(fs::view filePath)
    {
        json j;
        std::ifstream is;
        is.open(fs::view_util::get_view_path(filePath, true));
        is >> j;
        is.close();
        return deserialize(j);
    }
}
