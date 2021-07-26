#include <core/serialization/serializer.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename type>
    inline json serializer<type>::serialize(type data)
    {
        return json_view<component_prototype<type>>().serialize(component_prototype<type>(data));
    }

    template<typename type>
    inline component_prototype<type> serializer<type>::deserialize(json j)
    {
        return json_view<type>().deserialize(j);
    }

    template<typename type>
    inline void serializer<type>::write(fs::view filePath, type data)
    {
        std::ofstream os;
        os.open(fs::view_util::get_view_path(filePath,true));
        json j = serialize(data);
        os << j.dump();
        os.close();
    }

    template<typename type>
    inline component_prototype<type> serializer<type>::read(fs::view filePath)
    {
        json j;
        std::ifstream is;
        is.open(fs::view_util::get_view_path(filePath, true));
        is >> j;
        is.close();
        return deserialize(j);
    }
}
